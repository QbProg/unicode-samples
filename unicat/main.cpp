#include <QCoreApplication>
#include <QTextCodec>
#include <QStringList>
#include <QDebug>
#include <QFile>
#include <QTextStream>
#include <iostream>

/* NOTE : this is to enable unicode console output in Windows */
#ifdef _MSC_VER
	#include <fcntl.h>
	#include <io.h>
	#include <stdio.h>
#endif

int main(int argc, char *argv[])
{
    /* Command line usage and encoding list */
    if (argc < 2)
    {
        std::cout << "Usage : unicat <filename> --enc=<encoding> --detect-bom" << std::endl;
        std::cout << "Supported encodings:" << std::endl;

        Q_FOREACH(const QByteArray & B , QTextCodec::availableCodecs())
        {
            QString name(B);
            std::cout << name.toStdString() << std::endl;
        }
        return 1;
    }

    /* Command line parsing */
    QCoreApplication app (argc,argv);
    QStringList cmdline_args = QCoreApplication::arguments();
    cmdline_args.removeAt(0); // Removes app name
    QString encoding;
    QString fileName;
    bool detectBOM = false;

    Q_FOREACH(const QString & S ,cmdline_args)
    {
        if (S == "--detect-bom")
        {
            detectBOM = true;
        }
        else if (S.startsWith("--enc="))
        {
            encoding = S;
            encoding.remove(0,6); // removes --enc=
        }
        else if (fileName.isEmpty())
            fileName = S;
    }

    /* User coded selection */
    QTextCodec * userCodec = NULL;

    if (encoding.isEmpty())
    {
        userCodec = QTextCodec::codecForLocale();
    }
    else
    {
        userCodec = QTextCodec::codecForName(encoding.toAscii());
        if (userCodec == NULL)
        {
            std::cerr << "ERROR: could not load encoding " << encoding.toStdString() << std::endl;
            return 1;
        }
    }

    /* Debug informations */
    qDebug() << "Using encoding" << QString(userCodec->name());
    qDebug() << "FileName : " << QString(fileName);

    /* Open the file */
    QFile file(fileName);
    if (!file.open(QFile::ReadOnly|QFile::Text))
    {
        std::wcerr << L"ERROR: could not open filename " << fileName.toStdWString() << std::endl;
        return 1;
    }

    /* Setup the stream with the coded and BOM detection */
    QTextStream stream(&file);
    stream.setAutoDetectUnicode(detectBOM);
    stream.setCodec(userCodec);

#if _MSC_VER
	  _setmode(_fileno(stdout), _O_U16TEXT);  
#endif
    /* Read and write */
    while (!stream.atEnd())
    {
        QString s = stream.readLine();
        std::wcout << s.toStdWString() << std::endl;
    }

    file.close();

    return 0;
}
