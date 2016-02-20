#pragma once

class CFileSectionData;
class QString;
class CFileSection
{
public:
	CFileSection(const QString& filePath);
	~CFileSection();

    const QString& getNextSentence();
	const QString& fileContent();
private:
	CFileSectionData* m_d;
};

