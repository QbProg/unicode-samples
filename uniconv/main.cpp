#include <QCoreApplication>
#include <QTextCodec>
#include <QStringList>
#include <QDebug>
#include <QFile>
#include <QTextStream>
#include <iostream>

int main(int argc, char *argv[])
{
    /* Command line usage and encoding list */
    if (argc < 4)
    {
        std::cout << "Usage : uniconv <in_filename> <out_filename> --in_enc=<encoding> --out_enc=<encoding> [--detect-bom] [--no-write-bom]" << std::endl;
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
    QString in_encoding,out_encoding;
    QString in_fileName, out_fileName;
    bool detectBOM = false;
    bool writeBOM = true;

    Q_FOREACH(const QString & S ,cmdline_args)
    {
        if (S == "--detect-bom")
        {
            detectBOM = true;
        }
        else if (S == "--no-write-bom")
        {
            detectBOM = true;
        }
        else if (S.startsWith("--in_enc="))
        {
            in_encoding = S;
            in_encoding.remove(0,9); // removes --in_enc=
        }
        else if (S.startsWith("--out_enc="))
        {
            out_encoding = S;
            out_encoding.remove(0,10); // removes --out_enc=
        }
        else if (in_fileName.isEmpty())
            in_fileName = S;
        else
            out_fileName = S;
    }

    /* Input User coded selection */
    QTextCodec * userCodec = NULL;

    if (in_encoding.isEmpty())
    {
        userCodec = QTextCodec::codecForLocale();
    }
    else
    {
        userCodec = QTextCodec::codecForName(in_encoding.toAscii());
        if (userCodec == NULL)
        {
            std::cerr << "ERROR: could not load input encoding " <<in_encoding.toStdString() << std::endl;
            return 1;
        }
    }

    QTextCodec * outCodec = NULL;
    if (out_encoding.isEmpty())
    {
        outCodec = QTextCodec::codecForLocale();
    }
    else
    {
        outCodec = QTextCodec::codecForName(out_encoding.toAscii());
        if (outCodec == NULL)
        {
            std::cerr << "ERROR: could not load output encoding " << out_encoding.toStdString() << std::endl;
            return 1;
        }
    }

    /* Debug informations */
    qDebug() << "Using input encoding" << QString(userCodec->name());
    qDebug() << "Input FileName : " << QString(in_fileName);
    qDebug() << "Output FileName : " << QString(out_fileName);

    /* Open the input file */
    QFile inFile(in_fileName);
    if (!inFile.open(QFile::ReadOnly|QFile::Text))
    {
        std::wcerr << L"ERROR: could not open input filename " << in_fileName.toStdWString() << std::endl;
        return 1;
    }

    /* Open the output file */
    QFile outFile(out_fileName);
    if (!outFile.open(QFile::WriteOnly|QFile::Text))
    {
        std::wcerr << L"ERROR: could not open output filename " << out_fileName.toStdWString() << std::endl;
        return 1;
    }

    /* Setup the stream with the coded and BOM detection */
    QTextStream instream(&inFile);
    instream.setAutoDetectUnicode(detectBOM);
    instream.setCodec(userCodec);

    QTextStream outstream(&outFile);
    outstream.setCodec(outCodec);
    outstream.setGenerateByteOrderMark(writeBOM);

    /* Read and write */
    while (!instream.atEnd())
    {
        QString s = instream.readLine();
        outstream << s << "\n";
    }

    inFile.close();
    outFile.close();

    return 0;
}

