#include "text.h"
#include <functional>

QString printableString(const QByteArray &raw)
{
    QTextCodec::ConverterState state;
    QTextCodec *codec = QTextCodec::codecForName("UTF-8");
    const QString text = codec->toUnicode(raw.constData(), raw.size(), &state);

    if (state.invalidChars == 0
            && !raw.contains('\x00'))
        return text;

    QByteArray escapedBinaryString;
    char const hex[16] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
                           'A', 'B', 'C', 'D', 'E', 'F'};

    foreach (char i, raw) {
        if (isprint(i)) {
            escapedBinaryString.append(i);
        } else {
            escapedBinaryString.append("\\x");
            escapedBinaryString.append(&hex[(i  & 0xF0) >> 4], 1);
            escapedBinaryString.append(&hex[i & 0xF], 1);
        }
    }
    return escapedBinaryString;
}


bool isBinary(const QByteArray &raw)
{
    QTextCodec::ConverterState state;
    QTextCodec *codec = QTextCodec::codecForName("UTF-8");
    const QString text = codec->toUnicode(raw.constData(), raw.size(), &state);       
    return state.invalidChars != 0 || raw.contains('\x00');
}


QByteArray printableStringToBinary(const QString &str)
{
    QByteArray processedString;

    ushort quoteStarted = false;
    bool xFound = false;
    QByteArray hexBuff;

    std::function<void()> clear = [&quoteStarted, &xFound, &hexBuff]() {
        quoteStarted = xFound = false;
        hexBuff.clear();
    };

    for (QString::const_iterator i=str.constBegin(); i != str.constEnd(); ++i)
    {
        if (quoteStarted) {

            if (xFound) {
                if (('0' <= *i && *i <= '9')
                        || ('a' <= i->toLower() && i->toLower() <= 'f')) {

                    hexBuff.append(*i);

                    if (hexBuff.size() == 2) {
                        processedString.append(QByteArray::fromHex(hexBuff));
                        clear();
                    }

                } else {
                    processedString.append('\\');
                    processedString.append('x');
                    processedString.append(hexBuff);
                    clear();
                }
            } else {
                if (*i == 'x') {
                   xFound = true;
                   continue;
                } else {
                    processedString.append('\\');
                    processedString.append(*i);
                    clear();
                }
            }

        } else if (*i == '\\') {
            quoteStarted = true;
        } else {
            processedString.append(*i);
        }
    }

    return processedString;
}
