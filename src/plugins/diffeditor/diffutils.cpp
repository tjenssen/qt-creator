#include <utils/porting.h>
    return Utils::toSet(leftSelection).unite(Utils::toSet(rightSelection)).size();
    const int lineCount = lines.size();
    if (line < newLines.size()) {
    for (int line = 0; line < newLines.size(); ++line) {
        const int startPos = line > 0 ? -1 : lines->isEmpty() ? 0 : lines->last().text.size();
        const int endPos = line < newLines.size() - 1
                               ? -1
                               : lines->isEmpty() ? 0 : lines->last().text.size();
    while (i <= leftDiffList.size() && j <= rightDiffList.size()) {
        const Diff leftDiff = i < leftDiffList.size() ? leftDiffList.at(i) : Diff(Diff::Equal);
        const Diff rightDiff = j < rightDiffList.size() ? rightDiffList.at(j) : Diff(Diff::Equal);
            if (j == rightDiffList.size() && lastLineEqual && leftDiff.text.startsWith('\n'))
            if (j == rightDiffList.size())
            if (i == leftDiffList.size() && lastLineEqual && rightDiff.text.startsWith('\n'))
            if (i == leftDiffList.size())
            if (i < leftDiffList.size() || j < rightDiffList.size()
                || (!leftLines.isEmpty() && !rightLines.isEmpty())) {
                while (line < qMax(newLeftLines.size(), newRightLines.size())) {
                    const int commonLineCount = qMin(newLeftLines.size(), newRightLines.size());
                                if (i == leftDiffList.size() && j == rightDiffList.size())
                    if ((line < commonLineCount - 1)    // before the last common line in equality
                        || (line == commonLineCount - 1 // or the last common line in equality
                            && i == leftDiffList.size() // and it's the last iteration
                            && j == rightDiffList.size())) {
    for (int i = leftData.size(); i < rightData.size(); i++)
    for (int i = rightData.size(); i < leftData.size(); i++)
    const int visualLineCount = leftData.size();
    while (i < originalData.rows.size()) {
            while (i < originalData.rows.size()) {
            const bool last = i == originalData.rows.size(); // includes last line?
            const int lastLine = last ? originalData.rows.size() : i - contextLineCount;
    while (i < originalData.rows.size()) {
        while (i < originalData.rows.size()) {
        const int rowCount = chunkData.rows.size();
    for (int i = 0; i <= chunkData.rows.size(); i++) {
        const RowData &rowData = i < chunkData.rows.size()
                                     ? chunkData.rows.at(i)
                                     : RowData(TextLineData(TextLineData::Separator)); // dummy,
        // ensure we process buffers to the end.
        // rowData will be equal
                for (int j = 0; j < leftBuffer.size(); j++) {
                                                       leftBuffer.at(j).text,
                                                       lastChunk,
                                                       i == chunkData.rows.size()
                                                           && j == leftBuffer.size() - 1);
                for (int j = 0; j < rightBuffer.size(); j++) {
                                                       rightBuffer.at(j).text,
                                                       lastChunk,
                                                       i == chunkData.rows.size()
                                                           && j == rightBuffer.size() - 1);
            if (i < chunkData.rows.size()) {
                                                   rowData.rightLine.text,
                                                   lastChunk,
                                                   i == chunkData.rows.size() - 1);
    for (int i = 0; i < fileDataList.size(); i++) {
                for (int j = 0; j < fileData.chunks.size(); j++) {
                                     (j == fileData.chunks.size() - 1)
                                         && fileData.lastChunkAtTheEndOfFile);
static QList<RowData> readLines(StringView patch, bool lastChunk, bool *lastChunkAtTheEndOfFile, bool *ok)
    const QVector<StringView> lines = patch.split(newLine);
    for (i = 0; i < lines.size(); i++) {
        StringView line = lines.at(i);
                i = lines.size(); // pretend as we've read all the lines (we just ignore the rest)
                    noNewLineInEqual = diffList.size() - 1;
                    noNewLineInDelete = diffList.size() - 1;
                    noNewLineInInsert = diffList.size() - 1;
                    i = lines.size(); // pretend as we've read all the lines (we just ignore the rest)
                lastEqual = diffList.size() - 1;
                lastDelete = diffList.size() - 1;
                lastInsert = diffList.size() - 1;
    if (i < lines.size() // we broke before
        // or we have noNewLine in some equal line and in either delete or insert line
        || (noNewLineInEqual >= 0 && (noNewLineInDelete >= 0 || noNewLineInInsert >= 0))
        // or we have noNewLine in not the last equal line
        || (noNewLineInEqual >= 0 && noNewLineInEqual != lastEqual)
        // or we have noNewLine in not the last delete line or there is a equal line after the noNewLine for delete
        || (noNewLineInDelete >= 0 && (noNewLineInDelete != lastDelete || lastEqual > lastDelete))
        // or we have noNewLine in not the last insert line or there is a equal line after the noNewLine for insert
        || (noNewLineInInsert >= 0 && (noNewLineInInsert != lastInsert || lastEqual > lastInsert))) {
        diff.text = diff.text.left(diff.text.size() - 1);
        diff.text = diff.text.left(diff.text.size() - 1);
        diff.text = diff.text.left(diff.text.size() - 1);
static StringView readLine(StringView text, StringView *remainingText, bool *hasNewLine)
            *remainingText = StringView();
static bool detectChunkData(StringView chunkDiff, ChunkData *chunkData, StringView *remainingPatch)
    const StringView chunkLine = readLine(chunkDiff, remainingPatch, &hasNewLine);
    StringView leftPos = chunkLine.mid(leftPosStart, leftPosLength);
    StringView rightPos = chunkLine.mid(rightPosStart, rightPosLength);
    const StringView optionalHint = chunkLine.mid(optionalHintStart, optionalHintLength);
static QList<ChunkData> readChunks(StringView patch, bool *lastChunkAtTheEndOfFile, bool *ok)
    const int count = startingPositions.size();
                                 // drop the newline before the next chunk start
                                 ? startingPositions.at(i + 1) - 1
                                 // drop the possible newline by the end of patch
                                 : (patch.at(patch.size() - 1) == newLine ? patch.size() - 1
                                                                          : patch.size());
        const StringView chunkDiff = patch.mid(chunkStart, chunkEnd - chunkStart);
        StringView lines;
static FileData readDiffHeaderAndChunks(StringView headerAndChunks, bool *ok)
    StringView patch = headerAndChunks;
static QList<FileData> readDiffPatch(StringView patch, bool *ok, QFutureInterfaceBase *jobController)
                StringView headerAndChunks = patch.mid(lastPos, pos - lastPos);
            StringView headerAndChunks = patch.mid(lastPos, patch.size() - lastPos - 1);
static bool detectIndexAndBinary(StringView patch, FileData *fileData, StringView *remainingPatch)
    StringView afterNextLine;
    const StringView nextLine = readLine(patch, &afterNextLine, &hasNewLine);
        const StringView indices = nextLine.mid(indexHeader.size());
        *remainingPatch = StringView();
    StringView afterMinuses;
    const StringView minuses = readLine(*remainingPatch, &afterMinuses, &hasNewLine);
    StringView afterPluses;
    const StringView pluses = readLine(afterMinuses, &afterPluses, &hasNewLine);
static bool extractCommonFileName(StringView fileNames, StringView *fileName)
    if (!fileNames.startsWith(QLatin1String("a/")))
    if (!fileNames.mid(fileNameSize).startsWith(QLatin1String(" b/")))
    const StringView leftFileName = fileNames.mid(2, fileNameSize - 2);
    const StringView rightFileName = fileNames.mid(fileNameSize + 3, fileNameSize - 2);
static bool detectFileData(StringView patch, FileData *fileData, StringView *remainingPatch)
{
    StringView afterDiffGit;
    const StringView diffGit = readLine(patch, &afterDiffGit, &hasNewLine);
    const StringView fileNames = diffGit.mid(gitHeader.size());
    StringView commonFileName;
        StringView afterSecondLine;
        const StringView secondLine = readLine(afterDiffGit, &afterSecondLine, &hasNewLine);
            StringView afterThirdLine;
        StringView afterSimilarity;
        StringView afterCopyRenameFrom;
        const StringView copyRenameFrom = readLine(afterSimilarity, &afterCopyRenameFrom, &hasNewLine);
        StringView afterCopyRenameTo;
        const StringView copyRenameTo = readLine(afterCopyRenameFrom, &afterCopyRenameTo, &hasNewLine);
static QList<FileData> readGitPatch(StringView patch, bool *ok, QFutureInterfaceBase *jobController)
        StringView patch;
    const int count = startingPositions.size();
                                // drop the newline before the next header start
                                ? startingPositions.at(i + 1) - 1
                                // drop the possible newline by the end of file
                                : (patch.at(patch.size() - 1) == newLine ? patch.size() - 1
                                                                         : patch.size());
        const StringView fileDiff = patch.mid(diffStart, diffEnd - diffStart);
        StringView remainingFileDiff;
        jobController->setProgressRange(0, patches.size());
    StringView croppedPatch = make_stringview(patch);