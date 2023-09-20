// Copyright (C) 2022, Petros Koutsolampros

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include "documentmanager.h"

#include <QDir>
#include <QUrl>

DocumentManager::DocumentManager() {}

void DocumentManager::createEmptyDocument() {
    std::string newDocName = "Untitled";
    size_t counter = 1;
    while (std::find_if(m_openedDocuments.begin(), m_openedDocuments.end(),
                        NameDocumentComparator(newDocName)) != m_openedDocuments.end()) {
        newDocName = "Untitled " + std::to_string(counter);
        ++counter;
    }
    m_openedDocuments.push_back(
        std::make_pair(newDocName, std::unique_ptr<GraphModel>(new GraphModel(newDocName))));
    m_lastDocumentIndex = m_openedDocuments.size() - 1;
}

void DocumentManager::removeDocument(unsigned int index) {
    auto openDocument = m_openedDocuments.begin() + index;
    m_openedDocuments.erase(openDocument);
}

void DocumentManager::openDocument(QString urlString) {
    std::string fileName;
    const QUrl url(urlString);
    if (url.isLocalFile()) {
        fileName = QDir::toNativeSeparators(url.toLocalFile()).toStdString();
    } else {
        fileName = urlString.toStdString();
    }
    auto doc = std::find_if(m_openedDocuments.begin(), m_openedDocuments.end(),
                            NameDocumentComparator(fileName));
    if (doc != m_openedDocuments.end()) {
        m_lastDocumentIndex = std::distance(m_openedDocuments.begin(), doc);
        return;
    }

    m_openedDocuments.push_back(
        std::make_pair(fileName, std::unique_ptr<GraphModel>(new GraphModel(fileName))));
    m_lastDocumentIndex = m_openedDocuments.size() - 1;
}
