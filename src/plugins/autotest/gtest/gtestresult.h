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

#pragma once

#include "../testresult.h"

namespace Autotest {

class TestTreeItem;

namespace Internal {

class GTestResult : public TestResult
{
public:
    GTestResult(const QString &id, const Utils::FilePath &projectFile, const QString &name);
    const QString outputString(bool selected) const override;

    void setTestCaseName(const QString &testSetName) { m_testCaseName = testSetName; }
    void setIteration(int iteration) { m_iteration = iteration; }
    bool isDirectParentOf(const TestResult *other, bool *needsIntermediate) const override;
    virtual const ITestTreeItem *findTestTreeItem() const override;

private:
    bool isTestSuite() const { return m_testCaseName.isEmpty(); }
    bool isTestCase() const { return !m_testCaseName.isEmpty(); }

    bool matches(const TestTreeItem *item) const;
    bool matchesTestCase(const TestTreeItem *treeItem) const;
    bool matchesTestSuite(const TestTreeItem *treeItem) const;

    QString m_testCaseName;
    Utils::FilePath m_projectFile;
    int m_iteration = 1;
};

} // namespace Internal
} // namespace Autotest
