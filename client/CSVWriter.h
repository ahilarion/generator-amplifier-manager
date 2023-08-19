#ifndef CSVWRITER_H
#define CSVWRITER_H

#include <QString>
#include <QTextStream>
#include <QFile>
#include <QDir>

class CSVWriter {
public:
    CSVWriter() : file(nullptr) {}

    bool create(const QString& path, const QString& name) {
        QString filePath = QDir(path).filePath(name);

        QDir dir(QFileInfo(filePath).path());
        if (!dir.exists()) {
            if (!dir.mkpath(".")) {
                return false;
            }
        }

        file = new QFile(filePath);
        if (!file->open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) {
            delete file;
            file = nullptr;
            return false;
        }

        stream.setDevice(file);
        return true;
    }



    void close() {
        if (file) {
            file->close();
            delete file;
            file = nullptr;
        }
    }

    void write(const QStringList& values) {
        if (!file)
            return;

        stream << values.join(";") << "\n";
    }

private:
    QFile* file;
    QTextStream stream;
};


#endif // CSVWRITER_H
