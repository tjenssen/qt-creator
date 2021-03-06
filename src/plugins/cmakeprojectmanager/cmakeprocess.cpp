/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of Qt Creator.
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3 as published by the Free Software
** Foundation with exceptions as appearing in the file LICENSE.GPL3-EXCEPT
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-3.0.html.
**
****************************************************************************/

#include "cmakeprocess.h"

#include "cmakeparser.h"

#include <coreplugin/progressmanager/progressmanager.h>
#include <coreplugin/reaper.h>
#include <projectexplorer/buildsystem.h>
#include <projectexplorer/projectexplorerconstants.h>
#include <projectexplorer/taskhub.h>

#include <utils/stringutils.h>

using namespace Utils;

namespace CMakeProjectManager {
namespace Internal {

using namespace ProjectExplorer;

static QString lineSplit(const QString &rest, const QByteArray &array, std::function<void(const QString &)> f)
{
    QString tmp = rest + Utils::QtcProcess::normalizeNewlines(QString::fromLocal8Bit(array));
    int start = 0;
    int end = tmp.indexOf(QLatin1Char('\n'), start);
    while (end >= 0) {
        f(tmp.mid(start, end - start));
        start = end + 1;
        end = tmp.indexOf(QLatin1Char('\n'), start);
    }
    return tmp.mid(start);
}

CMakeProcess::CMakeProcess()
{
    connect(&m_cancelTimer, &QTimer::timeout, this, &CMakeProcess::checkForCancelled);
    m_cancelTimer.setInterval(500);
}

CMakeProcess::~CMakeProcess()
{
    if (m_process) {
        processStandardOutput();
        processStandardError();

        m_process->disconnect();
        Core::Reaper::reap(m_process.release());
    }

    m_parser.flush();

    if (m_future) {
        reportCanceled();
        reportFinished();
    }
}

void CMakeProcess::run(const BuildDirParameters &parameters, const QStringList &arguments)
{
    QTC_ASSERT(!m_process && !m_future, return);

    CMakeTool *cmake = parameters.cmakeTool();
    QTC_ASSERT(parameters.isValid() && cmake, return);

    const Utils::FilePath buildDirectory = parameters.buildDirectory;
    QTC_ASSERT(buildDirectory.exists(), return);

    const QString srcDir = parameters.sourceDirectory.path();

    const auto parser = new CMakeParser;
    parser->setSourceDirectory(srcDir);
    m_parser.addLineParser(parser);

    // Always use the sourceDir: If we are triggered because the build directory is getting deleted
    // then we are racing against CMakeCache.txt also getting deleted.

    auto process = std::make_unique<Utils::QtcProcess>();
    m_processWasCanceled = false;

    m_cancelTimer.start();

    process->setWorkingDirectory(buildDirectory);
    process->setEnvironment(parameters.environment);

    connect(process.get(), &QtcProcess::readyReadStandardOutput,
            this, &CMakeProcess::processStandardOutput);
    connect(process.get(), &QtcProcess::readyReadStandardError,
            this, &CMakeProcess::processStandardError);
    connect(process.get(), &QtcProcess::finished,
            this, &CMakeProcess::handleProcessFinished);

    CommandLine commandLine(cmake->cmakeExecutable(), QStringList({"-S", srcDir, "-B", buildDirectory.path()}) + arguments);

    TaskHub::clearTasks(ProjectExplorer::Constants::TASK_CATEGORY_BUILDSYSTEM);

    BuildSystem::startNewBuildSystemOutput(
        tr("Running %1 in %2.").arg(commandLine.toUserOutput()).arg(buildDirectory.toUserOutput()));

    auto future = std::make_unique<QFutureInterface<void>>();
    future->setProgressRange(0, 1);
    Core::ProgressManager::addTimedTask(*future.get(),
                                        tr("Configuring \"%1\"").arg(parameters.projectName),
                                        "CMake.Configure",
                                        10);

    process->setCommand(commandLine);
    emit started();
    m_elapsed.start();
    process->start();

    m_process = std::move(process);
    m_future = std::move(future);
}

QProcess::ProcessState CMakeProcess::state() const
{
    if (m_process)
        return m_process->state();
    return QProcess::NotRunning;
}

void CMakeProcess::reportCanceled()
{
    QTC_ASSERT(m_future, return);
    m_future->reportCanceled();
}

void CMakeProcess::reportFinished()
{
    QTC_ASSERT(m_future, return);
    m_future->reportFinished();
    m_future.reset();
}

void CMakeProcess::setProgressValue(int p)
{
    QTC_ASSERT(m_future, return);
    m_future->setProgressValue(p);
}

void CMakeProcess::processStandardOutput()
{
    QTC_ASSERT(m_process, return);

    static QString rest;
    rest = lineSplit(rest, m_process->readAllStandardOutput(), [](const QString &s) {
        BuildSystem::appendBuildSystemOutput(s);
    });
}

void CMakeProcess::processStandardError()
{
    QTC_ASSERT(m_process, return);

    static QString rest;
    rest = lineSplit(rest, m_process->readAllStandardError(), [this](const QString &s) {
        m_parser.appendMessage(s + '\n', Utils::StdErrFormat);
        BuildSystem::appendBuildSystemOutput(s);
    });
}

void CMakeProcess::handleProcessFinished(int code, QProcess::ExitStatus status)
{
    QTC_ASSERT(m_process && m_future, return);

    m_cancelTimer.stop();

    processStandardOutput();
    processStandardError();

    QString msg;
    if (status != QProcess::NormalExit) {
        if (m_processWasCanceled) {
            msg = tr("CMake process was canceled by the user.");
        } else {
            msg = tr("CMake process crashed.");
        }
    } else if (code != 0) {
        msg = tr("CMake process exited with exit code %1.").arg(code);
    }
    m_lastExitCode = code;

    if (!msg.isEmpty()) {
        BuildSystem::appendBuildSystemOutput(msg);
        TaskHub::addTask(BuildSystemTask(Task::Error, msg));
        m_future->reportCanceled();
    } else {
        m_future->setProgressValue(1);
    }

    m_future->reportFinished();

    emit finished();

    const QString elapsedTime = Utils::formatElapsedTime(m_elapsed.elapsed());
    BuildSystem::appendBuildSystemOutput(elapsedTime);
}

void CMakeProcess::checkForCancelled()
{
    if (!m_process || !m_future)
        return;

    if (m_future->isCanceled()) {
        m_cancelTimer.stop();
        m_processWasCanceled = true;
        m_process->close();
    }
}

} // namespace Internal
} // namespace CMakeProjectManager
