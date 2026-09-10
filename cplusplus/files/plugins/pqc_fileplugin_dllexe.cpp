/**************************************************************************
 **                                                                      **
 ** Copyright (C) 2011-2026 Lukas Spies                                  **
 ** Contact: https://photoqt.org                                         **
 **                                                                      **
 ** This file is part of PhotoQt.                                        **
 **                                                                      **
 ** PhotoQt is free software: you can redistribute it and/or modify      **
 ** it under the terms of the GNU General Public License as published by **
 ** the Free Software Foundation, either version 2 of the License, or    **
 ** (at your option) any later version.                                  **
 **                                                                      **
 ** PhotoQt is distributed in the hope that it will be useful,           **
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of       **
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        **
 ** GNU General Public License for more details.                         **
 **                                                                      **
 ** You should have received a copy of the GNU General Public License    **
 ** along with PhotoQt. If not, see <http://www.gnu.org/licenses/>.      **
 **                                                                      **
 **************************************************************************/

#include <fileplugins/pqc_fileplugin_dllexe.h>
#include <QCryptographicHash>
#include <QBuffer>

PQCFilePluginDLLExe::PQCFilePluginDLLExe() {

    setData({
        {44125,
            {{"Windows DLL"}, {"dll"}, {"application/vnd.microsoft.portable-executable"}}},
        {44126,
            {{"Windows Executable"}, {"exe"}, {"application/vnd.microsoft.portable-executable"}}},
             });


    // based on Microsoft's current LCID/LANGID tables
    // returns BCP-47-style locale name as those are unambiguous
    // see: https://learn.microsoft.com/en-us/openspecs/windows_protocols/ms-lcid/63d3d639-7fd2-4afb-abbe-0d5b5551eef8
    // entries starting with a colon (':') will be returned unchanged (and without colon)
    langId2Code = {

        // neutral / special
        {0x0000, ":Neutral"},
        {0x007f, ":Invariant"},

        // Arabic
        {0x0001, "ar"},
        {0x0401, "ar-SA"},
        {0x0801, "ar-IQ"},
        {0x0c01, "ar-EG"},
        {0x1001, "ar-LY"},
        {0x1401, "ar-DZ"},
        {0x1801, "ar-MA"},
        {0x1c01, "ar-TN"},
        {0x2001, "ar-OM"},
        {0x2401, "ar-YE"},
        {0x2801, "ar-SY"},
        {0x2c01, "ar-JO"},
        {0x3001, "ar-LB"},
        {0x3401, "ar-KW"},
        {0x3801, "ar-AE"},
        {0x3c01, "ar-BH"},
        {0x4001, "ar-QA"},
        {0x4401, "ar-SA"},
        {0x4801, "ar"},

        // Bulgarian
        {0x0002, "bg"},
        {0x0402, "bg-BG"},

        // Catalan
        {0x0003, "ca"},
        {0x0403, "ca-ES"},
        {0x0803, "ca-ES"},

        // Chinese
        {0x0004, "zh-Hans"},
        {0x0404, "zh-TW"},
        {0x0804, "zh-CN"},
        {0x0c04, "zh-HK"},
        {0x1004, "zh-SG"},
        {0x1404, "zh-MO"},
        {0x7804, "zh"},
        {0x7c04, "zh-Hant"},

        // Czech
        {0x0005, "cs"},
        {0x0405, "cs-CZ"},

        // Danish
        {0x0006, "da"},
        {0x0406, "da-DK"},

        // German
        {0x0007, "de"},
        {0x0407, "de-DE"},
        {0x0807, "de-CH"},
        {0x0c07, "de-AT"},
        {0x1007, "de-LU"},
        {0x1407, "de-LI"},

        // Greek
        {0x0008, "el"},
        {0x0408, "el-GR"},

        // English
        {0x0009, "en"},
        {0x0409, "en-US"},
        {0x0809, "en-GB"},
        {0x0c09, "en-AU"},
        {0x1009, "en-CA"},
        {0x1409, "en-NZ"},
        {0x1809, "en-IE"},
        {0x1c09, "en-ZA"},
        {0x2009, "en-JM"},
        {0x2409, "en-029"},
        {0x2809, "en-BZ"},
        {0x2c09, "en-TT"},
        {0x3009, "en-ZW"},
        {0x3409, "en-PH"},
        {0x3809, "en-ID"},
        {0x3c09, "en-HK"},
        {0x4009, "en-IN"},
        {0x4409, "en-MY"},
        {0x4809, "en-SG"},
        {0x4c09, "en-AE"},
        {0x5009, "en-BH"},
        {0x5409, "en-EG"},
        {0x5809, "en-JO"},
        {0x5c09, "en-KW"},
        {0x6009, "en-TR"},
        {0x6409, "en-YE"},

        // Spanish
        {0x000a, "es"},
        {0x040a, "es-ES"},
        {0x080a, "es-MX"},
        {0x0c0a, "es-ES"},
        {0x100a, "es-GT"},
        {0x140a, "es-CR"},
        {0x180a, "es-PA"},
        {0x1c0a, "es-DO"},
        {0x200a, "es-VE"},
        {0x240a, "es-CO"},
        {0x280a, "es-PE"},
        {0x2c0a, "es-AR"},
        {0x300a, "es-EC"},
        {0x340a, "es-CL"},
        {0x380a, "es-UY"},
        {0x3c0a, "es-PY"},
        {0x400a, "es-BO"},
        {0x440a, "es-SV"},
        {0x480a, "es-HN"},
        {0x4c0a, "es-NI"},
        {0x500a, "es-PR"},
        {0x540a, "es-US"},
        {0x580a, "es-419"},
        {0x5c0a, "es-CU"},

        // Finnish
        {0x000b, "fi"},
        {0x040b, "fi-FI"},

        // French
        {0x000c, "fr"},
        {0x040c, "fr-FR"},
        {0x080c, "fr-BE"},
        {0x0c0c, "fr-CA"},
        {0x100c, "fr-CH"},
        {0x140c, "fr-LU"},
        {0x180c, "fr-MC"},
        {0x1c0c, "fr-029"},
        {0x200c, "fr-RE"},
        {0x240c, "fr-CD"},
        {0x280c, "fr-SN"},
        {0x2c0c, "fr-CM"},
        {0x300c, "fr-CI"},
        {0x340c, "fr-ML"},
        {0x380c, "fr-MA"},
        {0x3c0c, "fr-HT"},
        {0xe40c, "fr-015"},

        // Hebrew
        {0x000d, "he"},
        {0x040d, "he-IL"},

        // Hungarian
        {0x000e, "hu"},
        {0x040e, "hu-HU"},

        // Icelandic
        {0x000f, "is"},
        {0x040f, "is-IS"},

        // Italian
        {0x0010, "it"},
        {0x0410, "it-IT"},
        {0x0810, "it-CH"},

        // Japanese
        {0x0011, "ja"},
        {0x0411, "ja-JP"},
        {0x0811, "ja-JP"},

        // Korean
        {0x0012, "ko"},
        {0x0412, "ko-KR"},

        // Dutch
        {0x0013, "nl"},
        {0x0413, "nl-NL"},
        {0x0813, "nl-BE"},

        // Norwegian
        {0x0014, "no"},
        {0x0414, "nb-NO"},
        {0x0814, "nn-NO"},
        {0x7814, "nn"},

        // Polish
        {0x0015, "pl"},
        {0x0415, "pl-PL"},

        // Portuguese
        {0x0016, "pt"},
        {0x0416, "pt-BR"},
        {0x0816, "pt-PT"},

        // Romansh
        {0x0017, "rm"},
        {0x0417, "rm-CH"},

        // Romanian
        {0x0018, "ro"},
        {0x0418, "ro-RO"},
        {0x0818, "ro-MD"},

        // Russian
        {0x0019, "ru"},
        {0x0419, "ru-RU"},
        {0x0819, "ru-MD"},

        // Croatian / Serbian / Bosnian
        {0x001a, ":Croatian"},
        {0x041a, "hr-HR"},
        {0x081a, "sr-Latn"},
        {0x0c1a, "sr-Cyrl"},
        {0x101a, "hr-BA"},
        {0x141a, "bs-Latn-BA"},
        {0x181a, "sr-Latn-BA"},
        {0x1c1a, "sr-Cyrl-BA"},
        {0x201a, "bs-Cyrl-BA"},
        {0x241a, "sr-Latn-RS"},
        {0x281a, "sr-Cyrl-RS"},
        {0x2c1a, "sr-Latn-ME"},
        {0x301a, "sr-Cyrl-ME"},
        {0x641a, "bs-Cyrl"},
        {0x681a, "bs-Latn"},
        {0x6c1a, "sr-Cyrl"},
        {0x701a, "sr-Latn"},
        {0x781a, "bs"},
        {0x7c1a, "sr"},

        // Slovak
        {0x001b, "sk"},
        {0x041b, "sk-SK"},

        // Albanian
        {0x001c, "sq"},
        {0x041c, "sq-AL"},

        // Swedish
        {0x001d, "sv"},
        {0x041d, "sv-SE"},
        {0x081d, "sv-FI"},

        // Thai
        {0x001e, "th"},
        {0x041e, "th-TH"},

        // Turkish
        {0x001f, "tr"},
        {0x041f, "tr-TR"},

        // Urdu
        {0x0020, "ur"},
        {0x0420, "ur-PK"},
        {0x0820, "ur-IN"},

        // Indonesian
        {0x0021, "id"},
        {0x0421, "id-ID"},

        // Ukrainian
        {0x0022, "uk"},
        {0x0422, "uk-UA"},

        // Belarusian
        {0x0023, "be"},
        {0x0423, "be-BY"},

        // Slovenian
        {0x0024, "sl"},
        {0x0424, "sl-SI"},

        // Estonian
        {0x0025, "et"},
        {0x0425, "et-EE"},

        // Latvian
        {0x0026, "lv"},
        {0x0426, "lv-LV"},

        // Lithuanian
        {0x0027, "lt"},
        {0x0427, "lt-LT"},

        // Tajik
        {0x0028, "tg"},
        {0x0428, "tg-Cyrl-TJ"},
        {0x7c28, "tg-Cyrl"},

        // Persian
        {0x0029, "fa"},
        {0x0429, "fa-IR"},

        // Vietnamese
        {0x002a, "vi"},
        {0x042a, "vi-VN"},

        // Armenian
        {0x002b, "hy"},
        {0x042b, "hy-AM"},

        // Azerbaijani
        {0x002c, "az"},
        {0x042c, "az-Latn-AZ"},
        {0x082c, "az-Cyrl-AZ"},
        {0x742c, "az-Cyrl"},
        {0x782c, "az-Latn"},

        // Basque
        {0x002d, "eu"},
        {0x042d, "eu-ES"},

        // Sorbian
        {0x002e, "hsb"},
        {0x042e, "hsb-DE"},
        {0x082e, "dsb-DE"},
        {0x7c2e, "dsb"},

        // Macedonian
        {0x002f, "mk"},
        {0x042f, "mk-MK"},

        // Southern Sotho
        {0x0030, "st"},
        {0x0430, "st-ZA"},

        // Tsonga
        {0x0031, "ts"},
        {0x0431, "ts-ZA"},

        // Tswana
        {0x0032, "tn"},
        {0x0432, "tn-ZA"},
        {0x0832, "tn-BW"},

        // Venda
        {0x0033, "ve"},
        {0x0433, "ve-ZA"},

        // Xhosa
        {0x0034, "xh"},
        {0x0434, "xh-ZA"},

        // Zulu
        {0x0035, "zu"},
        {0x0435, "zu-ZA"},

        // Afrikaans
        {0x0036, "af"},
        {0x0436, "af-ZA"},

        // Georgian
        {0x0037, "ka"},
        {0x0437, "ka-GE"},

        // Faroese
        {0x0038, "fo"},
        {0x0438, "fo-FO"},

        // Hindi
        {0x0039, "hi"},
        {0x0439, "hi-IN"},

        // Maltese
        {0x003a, "mt"},
        {0x043a, "mt-MT"},

        // Sami
        {0x003b, ":Sami"},
        {0x043b, "se-NO"},
        {0x083b, "se-SE"},
        {0x0c3b, "se-FI"},
        {0x103b, "smj-NO"},
        {0x143b, "smj-SE"},
        {0x183b, "sma-NO"},
        {0x1c3b, "sma-SE"},
        {0x203b, "sms-FI"},
        {0x243b, "smn-FI"},
        {0x703b, "smn"},
        {0x743b, "sms"},
        {0x783b, "sma"},
        {0x7c3b, "smj"},

        // Irish
        {0x003c, "ga"},
        {0x043c, "ga-IE"},
        {0x083c, "ga-IE"},

        // Yiddish
        {0x003d, "yi"},
        {0x043d, "yi-001"},

        // Malay
        {0x003e, "ms"},
        {0x043e, "ms-MY"},
        {0x083e, "ms-BN"},

        // Kazakh
        {0x003f, "kk"},
        {0x043f, "kk-KZ"},
        {0x083f, "kk-Latn-KZ"},
        {0x783f, "kk-Cyrl"},

        // Kyrgyz
        {0x0040, "ky"},
        {0x0440, "ky-KG"},

        // Swahili
        {0x0041, "sw"},
        {0x0441, "sw-KE"},

        // Turkmen
        {0x0042, "tk"},
        {0x0442, "tk-TM"},

        // Uzbek
        {0x0043, "uz"},
        {0x0443, "uz-Latn-UZ"},
        {0x0843, "uz-Cyrl-UZ"},
        {0x7843, "uz-Cyrl"},
        {0x7c43, "uz-Latn"},

        // Tatar
        {0x0044, "tt"},
        {0x0444, "tt-RU"},

        // Bengali / Bangla
        {0x0045, "bn"},
        {0x0445, "bn-IN"},
        {0x0845, "bn-BD"},

        // Punjabi
        {0x0046, "pa"},
        {0x0446, "pa-IN"},
        {0x0846, "pa-Arab-PK"},
        {0x7c46, "pa-Arab"},

        // Gujarati
        {0x0047, "gu"},
        {0x0447, "gu-IN"},

        // Odia
        {0x0048, "or"},
        {0x0448, "or-IN"},

        // Tamil
        {0x0049, "ta"},
        {0x0449, "ta-IN"},
        {0x0849, "ta-LK"},

        // Telugu
        {0x004a, "te"},
        {0x044a, "te-IN"},

        // Kannada
        {0x004b, "kn"},
        {0x044b, "kn-IN"},

        // Malayalam
        {0x004c, "ml"},
        {0x044c, "ml-IN"},

        // Assamese
        {0x004d, "as"},
        {0x044d, "as-IN"},

        // Marathi
        {0x004e, "mr"},
        {0x044e, "mr-IN"},

        // Sanskrit
        {0x004f, "sa"},
        {0x044f, "sa-IN"},

        // Mongolian
        {0x0050, "mn"},
        {0x0450, "mn-MN"},
        {0x0850, "mn-Mong-CN"},
        {0x0c50, "mn-Mong-MN"},
        {0x7850, "mn-Cyrl"},
        {0x7c50, "mn-Mong"},

        // Tibetan
        {0x0051, "bo"},
        {0x0451, "bo-CN"},
        {0x0851, "bo-BT"},

        // Welsh
        {0x0052, "cy"},
        {0x0452, "cy-GB"},

        // Khmer
        {0x0053, "km"},
        {0x0453, "km-KH"},

        // Lao
        {0x0054, "lo"},
        {0x0454, "lo-LA"},

        // Burmese
        {0x0055, "my"},
        {0x0455, "my-MM"},

        // Galician
        {0x0056, "gl"},
        {0x0456, "gl-ES"},

        // Konkani
        {0x0057, "kok"},
        {0x0457, "kok-IN"},

        // Manipuri
        {0x0058, ":Manipuri"},

        // Sindhi
        {0x0059, "sd"},
        {0x0859, "sd-Arab-PK"},
        {0x7c59, "sd-Arab"},

        // Syriac
        {0x005a, "syr"},
        {0x045a, "syr-SY"},

        // Sinhala
        {0x005b, "si"},
        {0x045b, "si-LK"},

        // Cherokee
        {0x005c, "chr"},
        {0x045c, "chr-Cher"},
        {0x7c5c, "chr-Cher"},

        // Inuktitut
        {0x005d, "iu"},
        {0x045d, "iu-Latn-CA"},
        {0x085d, "iu-Latn-CA"},
        {0x785d, "iu-Cans"},
        {0x7c5d, "iu-Latn"},

        // Amharic
        {0x005e, "am"},
        {0x045e, "am-ET"},

        // Tamazight
        {0x005f, "tzm"},
        {0x045f, "tzm-Latn-DZ"},
        {0x085f, "tzm-Latn-DZ"},
        {0x105f, "tzm-Tfng-MA"},
        {0x5c0f, "tzm"},
        {0x785f, "tzm-Tfng"},
        {0x7c5f, "tzm-Latn"},

        // Kashmiri
        {0x0060, "ks"},
        {0x0460, "ks-Deva-IN"},

        // Nepali
        {0x0061, "ne"},
        {0x0461, "ne-IN"},

        // Frisian
        {0x0062, "fy"},
        {0x0462, "fy-NL"},

        // Pashto
        {0x0063, "ps"},
        {0x0463, "ps-AF"},

        // Filipino
        {0x0064, "fil"},
        {0x0464, "fil-PH"},

        // Divehi
        {0x0065, "dv"},
        {0x0465, "dv-MV"},

        // Central European / pseudo language
        {0x0066, ":Central European"},

        // Fulah
        {0x0067, "ff"},
        {0x0867, "ff-Latn-SN"},
        {0x7c67, "ff-Latn"},

        // Hausa
        {0x0068, "ha"},
        {0x0468, "ha-Latn-NG"},
        {0x7c68, "ha-Latn"},

        // Yoruba
        {0x006a, "yo"},
        {0x046a, "yo-NG"},

        // Quechua
        {0x006b, "qu"},
        {0x046b, "quz-PE"},
        {0x086b, "quz-EC"},
        {0x0c6b, "quz-PE"},

        // Northern Sotho
        {0x006c, "nso"},
        {0x046c, "nso-ZA"},

        // Bashkir
        {0x006d, "ba"},
        {0x046d, "ba-RU"},

        // Luxembourgish
        {0x006e, "lb"},
        {0x046e, "lb-LU"},

        // Greenlandic
        {0x006f, "kl"},
        {0x046f, "kl-GL"},

        // Igbo
        {0x0070, "ig"},
        {0x0470, "ig-NG"},

        // Kanuri
        {0x0071, ":Kanuri"},

        // Oromo
        {0x0072, ":Oromo"},

        // Tigrinya
        {0x0073, "ti"},
        {0x0473, "ti-ER"},

        // Guarani
        {0x0074, ":Guarani"},

        // Hawaiian
        {0x0075, "haw"},
        {0x0475, "haw-US"},

        // Latin
        {0x0076, ":Latin"},

        // Somali
        {0x0077, "so"},
        {0x0477, "so-SO"},

        // Yi
        {0x0078, "ii"},
        {0x0478, "ii-CN"},

        // Papiamento
        {0x0079, ":Papiamento"},

        // Mapudungun
        {0x007a, "arn"},
        {0x047a, "arn-CL"},

        // Mohawk
        {0x007c, "moh"},
        {0x047c, "moh-CA"},

        // Breton
        {0x007e, "br"},
        {0x047e, "br-FR"},

        // Uyghur
        {0x0080, "ug"},
        {0x0480, "ug-CN"},

        // Māori
        {0x0081, "mi"},
        {0x0481, "mi-NZ"},

        // Occitan
        {0x0082, "oc"},
        {0x0482, "oc-FR"},

        // Corsican
        {0x0083, "co"},
        {0x0483, "co-FR"},

        // Alsatian
        {0x0084, "gsw"},
        {0x0484, "gsw-FR"},

        // Sakha / Yakut
        {0x0085, "sah"},
        {0x0485, "sah-RU"},

        // K'iche'
        {0x0086, "qut"},
        {0x0486, "qut-GT"},

        // Kinyarwanda
        {0x0087, "rw"},
        {0x0487, "rw-RW"},

        // Wolof
        {0x0088, "wo"},
        {0x0488, "wo-SN"},

        // Dari
        {0x008c, "prs"},
        {0x048c, "prs-AF"},

        // Scottish Gaelic
        {0x0091, "gd"},
        {0x0491, "gd-GB"},

        // Central Kurdish
        {0x0092, "ku"},
        {0x0492, "ku-Arab-IQ"},
        {0x7c92, "ku-Arab"},

        // reserved / pseudo locales appearing in Microsoft's LANGID tables
        {0x0501, ":Pseudo-localization"},
        {0x05fe, ":Pseudo-localization (mirrored)"},
        {0x09ff, ":Pseudo-localization (mirror)"},
        {0xf2ee, ":Reserved"},
        {0xeeee, ":Reserved"}
    };

}

const QVariantList PQCFilePluginDLLExe::loadData(QString path) {

    QFileInfo info(path);
    const QString mime = mimetypeForSupportedFile(path);
    if(mime.isEmpty() || !info.exists())
        return {};

    QVariantMap data;
    data["FileName"] = info.fileName();
    data["Filesize"] = info.size();
    data["TimeCreated"] = info.birthTime().toString();
    data["TimeModified"] = info.lastModified().toString();

    QFile file(path);
    if(!file.open(QIODevice::ReadOnly)) {
        qDebug() << "Unable to read file";
        return {true, data};
    }

    const QByteArray dllContent = file.readAll();
    if(dllContent.size() < 64)
        return {true, data};

    data["SHA256"] = QCryptographicHash::hash(dllContent, QCryptographicHash::Sha256).toHex();
    data["VersionInfo"] = parseDLLMetadata(path);

    return {true, data};

}

QVariantMap PQCFilePluginDLLExe::parseDLLMetadata(const QString path) {

    QVariantMap ret;

    QFile file(path);
    if(!file.open(QIODevice::ReadOnly)) {
        qWarning() << "unable to open DLL:" << path;
        return ret;
    }

    /******************************************************/
    // DOS header

    quint16 mzSignature = 0;

    if(!readUInt16LE(file, mzSignature) || mzSignature != 0x5A4D) { // "MZ"
        qDebug() << "not a valid PE/DLL file";
        ret["Architecture"] = "Not a valid PE/DLL file";
        return ret;
    }

    // e_lfanew is at offset 0x3C.
    if(!file.seek(0x3C))
        return ret;

    quint32 eLfanew = 0;

    if(!readUInt32LE(file, eLfanew)) {
        qDebug() << "unable to read e_lfanew";
        return ret;
    }

    /******************************************************/
    // PE signature

    if(!file.seek(eLfanew)) {
        qDebug() << "unable to seek to e_lfanew";
        return ret;
    }

    quint32 peSignature = 0;

    if(!readUInt32LE(file, peSignature) || peSignature != 0x00004550) { // "PE\0\0"
        qDebug() << "wrong PE signature";
        return ret;
    }

    /******************************************************/
    // COFF file header

    quint16 machine = 0;
    quint16 numberOfSections = 0;
    quint32 timeDateStamp = 0;
    quint32 pointerToSymbolTable = 0;
    quint32 numberOfSymbols = 0;
    quint16 sizeOptionalHeader = 0;
    quint16 characteristics = 0;

    if(!readUInt16LE(file, machine)               || !readUInt16LE(file, numberOfSections) || !readUInt32LE(file, timeDateStamp) ||
        !readUInt32LE(file, pointerToSymbolTable) || !readUInt32LE(file, numberOfSymbols)  || !readUInt16LE(file, sizeOptionalHeader) ||
        !readUInt16LE(file, characteristics)) {
        qDebug() << "invalid COFF header";
        return ret;
    }

    Q_UNUSED(pointerToSymbolTable);
    Q_UNUSED(numberOfSymbols);
    Q_UNUSED(characteristics);

    /******************************************************/
    // architecture

    if(machine == 0x014C)
        ret["Architecture"] = "x86 (32-bit)";
    else if(machine == 0x8664)
        ret["Architecture"] = "x86_64 (64-bit)";
    else if(machine == 0x01C0)
        ret["Architecture"] = "ARM (32-bit)";
    else if(machine == 0xAA64)
        ret["Architecture"] = "ARM64 (64-bit)";
    else
        ret["Architecture"] = QString("Unknown (Machine: %1)").arg(machine, 0, 16);

    ret["Timestamp"] = QDateTime::fromSecsSinceEpoch(timeDateStamp).toString();

    /******************************************************/
    // optional header

    const quint64 optionalHeaderOffset = quint64(eLfanew) + 4 + 20;

    if(!file.seek(optionalHeaderOffset)) {
        qDebug() << "unable to seek to optional header";
        return ret;
    }

    quint16 optionalMagic = 0;

    if(!readUInt16LE(file, optionalMagic)) {
        qDebug() << "unable to seek to read optional magic";
        return ret;
    }

    quint32 entryPoint = 0;
    quint64 imageBase = 0;
    quint64 dataDirectoryOffset = 0;

    // PE32
    if(optionalMagic == 0x10B) {

        if(!file.seek(optionalHeaderOffset + 16) || !readUInt32LE(file, entryPoint)) {
            qDebug() << "unable to read entry_point";
            return ret;
        }

        quint32 imageBase32 = 0;

        if(!file.seek(optionalHeaderOffset + 28) || !readUInt32LE(file, imageBase32)) {
            qDebug() << "unable to read image_base (32)";
            return ret;
        }

        // IMAGE_DATA_DIRECTORY starts at +96 for PE32
        dataDirectoryOffset = 96;

    // PE32+
    } else if(optionalMagic == 0x20B) {

            if(!file.seek(optionalHeaderOffset + 16) || !readUInt32LE(file, entryPoint)) {
                qDebug() << "unable to read entry_point";
                return ret;
            }

            if(!file.seek(optionalHeaderOffset + 24) || !readUInt64LE(file, imageBase)) {
                qDebug() << "unable to read image_base";
                return ret;
            }

            // IMAGE_DATA_DIRECTORY starts at +112 for PE32+
            dataDirectoryOffset = 112;

    } else {

            qDebug() << "unknown PE optional header:" << Qt::hex << optionalMagic;
            return ret;

    }

    /******************************************************/
    // Resource Directory

    // IMAGE_DIRECTORY_ENTRY_RESOURCE
    const quint64 resourceDirectoryOffset = optionalHeaderOffset + dataDirectoryOffset + 2 * 8;

    quint32 resourceRva = 0;
    quint32 resourceSize = 0;

    if(!file.seek(resourceDirectoryOffset) || !readUInt32LE(file, resourceRva) || !readUInt32LE(file, resourceSize)) {
        resourceRva = 0;
        resourceSize = 0;
    }

    /******************************************************/
    // Section Headers

    QList<PESection> sections;
    QStringList sectionNames;

    const quint64 sectionTableOffset = quint64(eLfanew) + 4 + 20 + sizeOptionalHeader;

    if(!file.seek(sectionTableOffset)) {
        qDebug() << "unable to seek to section table offset";
        return ret;
    }

    for(quint16 i = 0; i < numberOfSections; ++i) {

        QByteArray sectionName;

        if(!readBytes(file, 8, sectionName)) {
            qDebug() << "unable to read section_name";
            return ret;
        }

        const qsizetype nullPos = sectionName.indexOf('\0');

        if(nullPos >= 0)
            sectionName.truncate(nullPos);

        sectionNames.append(QString::fromLatin1(sectionName).trimmed());

        PESection section;

        if(!readUInt32LE(file, section.virtualSize) || !readUInt32LE(file, section.virtualAddress) ||
            !readUInt32LE(file, section.rawSize)    || !readUInt32LE(file, section.rawAddress)) {
            qDebug() << "unable to read section related information";
            return ret;
        }

        // PointerToRelocations
        quint32 value32 = 0;

        if(!readUInt32LE(file, value32)) {
            qDebug() << "unable to read PointerToRelocations";
            return ret;
        }

        // PointerToLinenumbers
        if(!readUInt32LE(file, value32)) {
            qDebug() << "unable to read PointerToLinenumbers";
            return ret;
        }

        // NumberOfRelocations
        quint16 value16 = 0;

        if(!readUInt16LE(file, value16)) {
            qDebug() << "unable to read NumberOfRelocations";
            return ret;
        }

        // NumberOfLinenumbers
        if(!readUInt16LE(file, value16)) {
            qDebug() << "unable to NumberOfLinenumbers";
            return ret;
        }

        // Characteristics
        if(!readUInt32LE(file, value32)) {
            qDebug() << "unable to read Characteristics";
            return ret;
        }

        sections.append(section);
    }

    ret["Sections"] = sectionNames;

    /******************************************************/
    // find VERSION resource

    if(resourceRva == 0 || resourceSize == 0) {
        qDebug() << "no VERSION resource found";
        return ret;
    }

    quint64 resourceFileOffset = 0;

    if(!rvaToFileOffset(resourceRva, sections, resourceFileOffset)) {
        qDebug() << "call to rvaToFileOffset() failed";
        return ret;
    }

    QList<ResourceEntry> rootEntries;

    if(!readResourceDirectoryEntries(file, resourceFileOffset, rootEntries)) {
        qDebug() << "call to readResourceDirectoryEntries failed";
        return ret;
    }

    /******************************************************/
    // find any embedded icon

    QImage icon;

    if(!extractBestIcon(file, resourceFileOffset, rootEntries, sections, icon))
        qDebug() << "no usable icon found";
    else {

        QByteArray bytes;
        QBuffer buffer(&bytes);

        if(!buffer.open(QIODevice::WriteOnly)) {
            qDebug() << "Unable to open buffer for icon";
        } else {
            if(!icon.save(&buffer, "PNG"))
                qDebug() << "unable to save icon as png to buffer";
            else
                ret["Icon"] = QString::fromLatin1(bytes.toBase64());
        }

    }

    /******************************************************/
    // find RT_VERSION (resource ID 16)

    ResourceEntry versionType;
    bool foundVersion = false;

    for(const ResourceEntry& entry : std::as_const(rootEntries)) {
        if(!entry.isNamed() && entry.id() == 16 && entry.isDirectory()) {
            versionType = entry;
            foundVersion = true;
            break;
        }
    }

    if(!foundVersion) {
        qDebug() << "RT_VERSION not found";
        return ret;
    }

    // -------------------------------------------------------------------------
    // Version resource ID
    // -------------------------------------------------------------------------

    QList<ResourceEntry> versionEntries;

    if(!readResourceDirectoryEntries(file, resourceFileOffset + versionType.directoryOffset(), versionEntries)) {
        qDebug() << "call to readResourceDirectoryEntries failed";
        return ret;
    }

    if(versionEntries.isEmpty()) {
        qDebug() << "no version entries found";
        return ret;
    }

    const ResourceEntry versionResource = versionEntries.first();

    if(!versionResource.isDirectory()) {
        qDebug() << "version resource not a dir";
        return ret;
    }

    /******************************************************/
    // Language

    QList<ResourceEntry> languageEntries;

    if(!readResourceDirectoryEntries(file, resourceFileOffset + versionResource.directoryOffset(), languageEntries)) {
        qDebug() << "call to readResourceDirectoryEntries failed";
        return ret;
    }

    if(languageEntries.isEmpty()) {
        qDebug() << "no language entries found";
        return ret;
    }

    const ResourceEntry language = languageEntries.first();

    if(language.isDirectory()) {
        qDebug() << "language is a dir";
        return ret;
    }

    /******************************************************/
    // IMAGE_RESOURCE_DATA_ENTRY

    const quint64 dataEntryOffset = resourceFileOffset + language.offset;

    if(!file.seek(dataEntryOffset)) {
        qDebug() << "failed to seek to datae entry offset";
        return ret;
    }

    quint32 versionDataRva = 0;
    quint32 versionDataSize = 0;
    quint32 codePage = 0;
    quint32 reserved = 0;

    if(!readUInt32LE(file, versionDataRva) || !readUInt32LE(file, versionDataSize) ||
        !readUInt32LE(file, codePage) || !readUInt32LE(file, reserved)) {
        qDebug() << "failed to read version data info";
        return ret;
    }

    Q_UNUSED(reserved);

    quint64 versionFileOffset = 0;

    if(!rvaToFileOffset(versionDataRva, sections, versionFileOffset)) {
        qDebug() << "call to rvaToFileOffset failed";
        return ret;
    }

    /******************************************************/
    // Parse VS_VERSION_INFO

    if(!parseVersionBlock(file, versionFileOffset, versionDataSize, ret, language.id(), codePage))
        qDebug() << "unable to parse VERSIONINFO";

    return ret;

}

/***************************************************************/

bool PQCFilePluginDLLExe::parseVersionBlock(QFile& file, quint64 blockOffset, quint64 availableSize, QVariantMap& ret, quint32 languageId, quint32 codePage) {

    if(availableSize < 6) {
        qDebug() << "availableSize to small:" << availableSize;
        return false;
    }

    if(!file.seek(blockOffset)) {
        qDebug() << "failed to seek to block offset:" << blockOffset;
        return false;
    }

    quint16 length = 0;
    quint16 valueLength = 0;
    quint16 type = 0;

    if(!readUInt16LE(file, length) || !readUInt16LE(file, valueLength) || !readUInt16LE(file, type)) {
        qDebug() << "failed to read length/valueLength/type";
        return false;
    }

    // a VS_VERSIONINFO block must at least contain:
    // - WORD wLength
    // - WORD wValueLength
    // - WORD wType
    if(length < 6 || quint64(length) > availableSize) {
        qDebug() << "length/availableSize failed check:" << length << "/" << availableSize;
        return false;
    }

    const quint64 actualBlockEnd = blockOffset + quint64(length);

    /***************************************************************/
    // Key

    QString key;

    if(!readUtf16String(file, key)) {
        qDebug() << "failed to read key";
        return false;
    }

    const quint64 valueOffset = align4(quint64(file.pos()));

    if(valueOffset > actualBlockEnd) {
        qDebug() << "value appears to be beyond block end:" << valueOffset << "/" << actualBlockEnd;
        return false;
    }

    /***************************************************************/
    // VS_FIXEDFILEINFO

    if(type == 0 && key == "VS_VERSION_INFO") {

        // VS_FIXEDFILEINFO is 13 DWORDs = 52 bytes.

        constexpr quint64 fixedFileInfoSize = 52;

        if(valueLength >= 52 && valueOffset + fixedFileInfoSize <= actualBlockEnd) {

            if(!file.seek(valueOffset)) {
                qDebug() << "failed to seek to value offset:" << valueOffset;
                return false;
            }

            quint32 signature = 0;
            quint32 structureVersion = 0;
            quint32 fileVersionMS = 0;
            quint32 fileVersionLS = 0;
            quint32 productVersionMS = 0;
            quint32 productVersionLS = 0;
            quint32 fileFlagsMask = 0;
            quint32 fileFlags = 0;
            quint32 fileOS = 0;
            quint32 fileType = 0;
            quint32 fileSubtype = 0;
            quint32 fileDateMS = 0;
            quint32 fileDateLS = 0;

            if(!readUInt32LE(file, signature)      || !readUInt32LE(file, structureVersion) || !readUInt32LE(file, fileVersionMS) ||
                !readUInt32LE(file, fileVersionLS) || !readUInt32LE(file, productVersionMS) || !readUInt32LE(file, productVersionLS) ||
                !readUInt32LE(file, fileFlagsMask) || !readUInt32LE(file, fileFlags)        || !readUInt32LE(file, fileOS) ||
                !readUInt32LE(file, fileType)      || !readUInt32LE(file, fileSubtype)      || !readUInt32LE(file, fileDateMS) ||
                !readUInt32LE(file, fileDateLS)) {
                qDebug() << "failed to read various quint32";
                return false;
            }

            // VS_FFI_SIGNATURE
            if(signature == 0xFEEF04BDu) {

                ret["FileVersion"] = formatVersion(fileVersionMS, fileVersionLS);
                ret["ProductVersion"] = formatVersion(productVersionMS, productVersionLS);

                ret["FileFlagsMask"] = formatFileFlagsMask(fileFlagsMask);
                ret["FileFlags"] = formatFileFlags(fileFlags);
                ret["FileOS"] = formatFileOs(fileOS);
                ret["FileType"] = formatFileType(fileType);
                ret["FileSubtype"] = formatFileSubtype(fileType, fileSubtype);

                ret["StructureVersion"] = formatStructureVersion(structureVersion);
                ret["VersionLanguage"] = formatLanguageName(languageId);
                ret["VersionCodePage"] = formatCodePage(codePage);

                ret["FileDate"] = formatFileDate(fileDateMS, fileDateLS);
            }
        }

    /***************************************************************/
    // String values

    // A VERSIONINFO string block has:
    //
    // - wType        = 1
    // - wValueLength = number of UTF-16 characters
    //
    // => value occupies wValueLength * 2 bytes

    } else if(type == 1 && valueLength > 0) {

        const quint64 maxBytes = actualBlockEnd > valueOffset ? (actualBlockEnd - valueOffset) : 0;
        const quint64 requestedBytes = quint64(valueLength) * 2;
        const quint64 bytesToRead = qMin(requestedBytes, maxBytes);

        if(bytesToRead > 0) {

            if(!file.seek(valueOffset)) {
                qDebug() << "failed to seek to value offset";
                return false;
            }

            QByteArray raw;

            if(!readBytes(file, static_cast<qsizetype>(bytesToRead), raw)) {
                qDebug() << "failed to read raw bytes:" << bytesToRead;
                return false;
            }

            const qsizetype charCount = raw.size() / 2;

            QVector<char16_t> chars;
            chars.reserve(charCount);

            for(qsizetype i = 0; i < charCount; ++i) {
                const uchar* p = reinterpret_cast<const uchar*>(raw.constData() + i * 2);
                const quint16 ch = (quint16(p[0]) | (quint16(p[1]) << 8));
                chars.append(static_cast<char16_t>(ch));
            }

            QString value = QString::fromUtf16(chars.constData(), chars.size());

            // some VERSIONINFO resources include a terminating \0
            // even though wValueLength already accounts for it
            const qsizetype nul = value.indexOf(QChar(u'\0'));

            if(nul >= 0)
                value.truncate(nul);

            if(!key.isEmpty()) {

                // FileVersion and ProductVersion from VS_FIXEDFILEINFO
                // are preferred over their StringFileInfo counterparts

                const QSet<QString> lookFor = {"Comments", "CompanyName", "FileDescription", "LegalCopyright", "OriginalFilename", "ProductName"};
                if(lookFor.contains(key))
                    ret[key] = value;

            }
        }
    }

    /***************************************************************/
    // Children

    const quint64 valueBytes = (type == 1 ? quint64(valueLength) * 2 : valueLength);

    // Children start after the value and are aligned to a DWORD boundary.

    quint64 childOffset = align4(valueOffset + valueBytes);

    while(childOffset + 6 <= actualBlockEnd) {

        if(!file.seek(childOffset)) {
            qDebug() << "failed to seek to child offset:" << childOffset;
            return false;
        }

        quint16 childLength = 0;

        if(!readUInt16LE(file, childLength)) {
            qDebug() << "failed to read childLength";
            return false;
        }

        // a child block must have at least the 6-byte header and must
        // fit entirely inside its parent

        if(childLength < 6 || childOffset + quint64(childLength) > actualBlockEnd)
            break;

        if(!parseVersionBlock(file, childOffset, childLength, ret, languageId, codePage)) {
            qDebug() << "recursive call to parseVersionBlock failed";
            return false;
        }

        // Each child occupies a DWORD-aligned size in its parent.
        childOffset += align4(quint64(childLength));

    }

    return true;

}

bool PQCFilePluginDLLExe::extractBestIcon(QFile& file, quint64 resourceFileOffset, const QList<ResourceEntry>& rootEntries, const QList<PESection>& sections, QImage& icon) {

    /******************************************************/
    // locate RT_GROUP_ICON (resource ID 14)

    ResourceEntry groupIconType;
    bool foundGroupIconType = false;

    for(const ResourceEntry& entry : rootEntries) {
        if(!entry.isNamed() && entry.id() == 14 && entry.isDirectory()) {
            groupIconType = entry;
            foundGroupIconType = true;
            break;
        }
    }

    if(!foundGroupIconType) {
        qDebug() << "RT_GROUP_ICON not found";
        return false;
    }

    QList<ResourceEntry> groupNameEntries;

    if(!readResourceDirectoryEntries(file, resourceFileOffset + groupIconType.directoryOffset(), groupNameEntries) || groupNameEntries.isEmpty()) {
        qDebug() << "no RT_GROUP_ICON name entries found";
        return false;
    }

    // take first icon group
    // most exe/dll files only ship one
    const ResourceEntry groupName = groupNameEntries.first();

    if(!groupName.isDirectory()) {
        qDebug() << "RT_GROUP_ICON name entry is not a directory";
        return false;
    }

    QList<ResourceEntry> groupLanguageEntries;

    if(!readResourceDirectoryEntries(file, resourceFileOffset + groupName.directoryOffset(), groupLanguageEntries) || groupLanguageEntries.isEmpty()) {
        qDebug() << "no RT_GROUP_ICON language entries found";
        return false;
    }

    const ResourceEntry groupLanguage = groupLanguageEntries.first();

    if(groupLanguage.isDirectory()) {
        qDebug() << "RT_GROUP_ICON language entry is a directory";
        return false;
    }

    /******************************************************/
    // IMAGE_RESOURCE_DATA_ENTRY for the icon group (GRPICONDIR)

    if(!file.seek(resourceFileOffset + groupLanguage.offset)) {
        qDebug() << "failed to seek to RT_GROUP_ICON data entry";
        return false;
    }

    quint32 groupRva = 0;
    quint32 groupSize = 0;
    quint32 groupCodePage = 0;
    quint32 groupReserved = 0;

    if(!readUInt32LE(file, groupRva) || !readUInt32LE(file, groupSize) || !readUInt32LE(file, groupCodePage) || !readUInt32LE(file, groupReserved)) {
        qDebug() << "failed to read RT_GROUP_ICON data info";
        return false;
    }

    Q_UNUSED(groupCodePage);
    Q_UNUSED(groupReserved);

    if(groupSize < 6) {
        qDebug() << "GRPICONDIR too small:" << groupSize;
        return false;
    }

    quint64 groupFileOffset = 0;

    if(!rvaToFileOffset(groupRva, sections, groupFileOffset)) {
        qDebug() << "call to rvaToFileOffset failed for GRPICONDIR";
        return false;
    }

    if(!file.seek(groupFileOffset)) {
        qDebug() << "failed to seek to GRPICONDIR";
        return false;
    }

    /******************************************************/
    // GRPICONDIR header (== NEWHEADER)

    quint16 giReserved = 0;
    quint16 giType = 0;
    quint16 giCount = 0;

    if(!readUInt16LE(file, giReserved) || !readUInt16LE(file, giType) || !readUInt16LE(file, giCount)) {
        qDebug() << "failed to read GRPICONDIR header";
        return false;
    }

    if(giType != 1 || giCount == 0) {
        qDebug() << "invalid GRPICONDIR header, type/count:" << giType << giCount;
        return false;
    }

    /******************************************************/
    // GRPICONDIRENTRY array

    // we pick the entry with the largest area or, if more than one found,
    // pick the one with the highest bit depth

    const quint64 maxEntries = (quint64(groupSize) - 6) / 14;
    const quint16 entryCount = quint16(qMin<quint64>(giCount, maxEntries));

    quint16 bestId = 0;
    quint64 bestScore = 0;
    bool haveBest = false;

    for(quint16 i = 0; i < entryCount; ++i) {

        QByteArray header;

        if(!readBytes(file, 4, header)) {
            qDebug() << "failed to read GRPICONDIRENTRY header" << i;
            return false;
        }

        const quint8 width  = quint8(header.at(0));
        const quint8 height = quint8(header.at(1));

        quint16 planes = 0;
        quint16 bitCount = 0;
        quint32 bytesInRes = 0;
        quint16 id = 0;

        if(!readUInt16LE(file, planes) || !readUInt16LE(file, bitCount) || !readUInt32LE(file, bytesInRes) || !readUInt16LE(file, id)) {
            qDebug() << "failed to read GRPICONDIRENTRY" << i;
            return false;
        }

        Q_UNUSED(planes);
        Q_UNUSED(bytesInRes);

        const quint64 realWidth  = (width  == 0 ? 256 : width);
        const quint64 realHeight = (height == 0 ? 256 : height);
        const quint64 score = realWidth * realHeight * quint64(qMax<quint16>(bitCount, 1));

        if(!haveBest || score > bestScore) {
            bestScore = score;
            bestId = id;
            haveBest = true;
        }
    }

    if(!haveBest) {
        qDebug() << "no usable GRPICONDIRENTRY found";
        return false;
    }

    /******************************************************/
    // find the matching RT_ICON (resource ID 3) entry

    ResourceEntry iconType;
    bool foundIconType = false;

    for(const ResourceEntry& entry : rootEntries) {
        if(!entry.isNamed() && entry.id() == 3 && entry.isDirectory()) {
            iconType = entry;
            foundIconType = true;
            break;
        }
    }

    if(!foundIconType) {
        qDebug() << "RT_ICON not found";
        return false;
    }

    QList<ResourceEntry> iconNameEntries;

    if(!readResourceDirectoryEntries(file, resourceFileOffset + iconType.directoryOffset(), iconNameEntries)) {
        qDebug() << "failed to read RT_ICON name entries";
        return false;
    }

    ResourceEntry iconName;
    bool foundIconName = false;

    for(const ResourceEntry& entry : std::as_const(iconNameEntries)) {
        if(!entry.isNamed() && entry.id() == bestId && entry.isDirectory()) {
            iconName = entry;
            foundIconName = true;
            break;
        }
    }

    if(!foundIconName) {
        qDebug() << "matching RT_ICON id not found:" << bestId;
        return false;
    }

    QList<ResourceEntry> iconLanguageEntries;

    if(!readResourceDirectoryEntries(file, resourceFileOffset + iconName.directoryOffset(), iconLanguageEntries) ||
        iconLanguageEntries.isEmpty()) {
        qDebug() << "no RT_ICON language entries found";
        return false;
    }

    const ResourceEntry iconLanguage = iconLanguageEntries.first();

    if(iconLanguage.isDirectory()) {
        qDebug() << "RT_ICON language entry is a directory";
        return false;
    }

    /******************************************************/
    // IMAGE_RESOURCE_DATA_ENTRY for the actual icon image

    if(!file.seek(resourceFileOffset + iconLanguage.offset)) {
        qDebug() << "failed to seek to RT_ICON data entry";
        return false;
    }

    quint32 iconRva = 0;
    quint32 iconSize = 0;
    quint32 iconCodePage = 0;
    quint32 iconReserved = 0;

    if(!readUInt32LE(file, iconRva) || !readUInt32LE(file, iconSize) || !readUInt32LE(file, iconCodePage) || !readUInt32LE(file, iconReserved)) {
        qDebug() << "failed to read RT_ICON data info";
        return false;
    }

    Q_UNUSED(iconCodePage);
    Q_UNUSED(iconReserved);

    if(iconSize == 0) {
        qDebug() << "RT_ICON image data is empty";
        return false;
    }

    quint64 iconFileOffset = 0;

    if(!rvaToFileOffset(iconRva, sections, iconFileOffset)) {
        qDebug() << "call to rvaToFileOffset failed for icon image data";
        return false;
    }

    if(!file.seek(iconFileOffset)) {
        qDebug() << "failed to seek to icon image data";
        return false;
    }

    QByteArray iconData;

    if(!readBytes(file, qsizetype(iconSize), iconData)) {
        qDebug() << "failed to read icon image data";
        return false;
    }

    /******************************************************/
    // assemble in-memory .ico file
    // this handles both BMP/DIB icon frame and PNG-encoded frames

    QByteArray icoFile;
    QDataStream stream(&icoFile, QIODevice::WriteOnly);
    stream.setByteOrder(QDataStream::LittleEndian);

    // reserved
    stream << quint16(0);
    // type: 1 = icon
    stream << quint16(1);
    // number of images in this file
    stream << quint16(1);

    // width (0 == 256; real size is in the image data)
    stream << quint8(0);
    // height (0 == 256)
    stream << quint8(0);
    // color count
    stream << quint8(0);
    // reserved
    stream << quint8(0);
    // planes
    stream << quint16(1);
    // bit count
    stream << quint16(32);
    // size of image data
    stream << quint32(iconData.size());
    // offset to image data: 6-byte ICONDIR + 16-byte ICONDIRENTRY
    stream << quint32(22);

    icoFile.append(iconData);

    icon = QImage::fromData(icoFile, "ICO");

    if(icon.isNull()) {
        qDebug() << "failed to decode reassembled .ico data";
        return false;
    }

    return true;
}

/***************************************************************/

// various helper functions for the parser above

bool PQCFilePluginDLLExe::readUInt16LE(QFile& file, quint16& value) {

    const QByteArray data = file.read(2);
    if(data.size() != 2)
        return false;

    value = qFromLittleEndian<quint16>(reinterpret_cast<const uchar*>(data.constData()));

    return true;

}

bool PQCFilePluginDLLExe::readUInt32LE(QFile& file, quint32& value) {

    const QByteArray data = file.read(4);
    if(data.size() != 4)
        return false;

    value = qFromLittleEndian<quint32>(reinterpret_cast<const uchar*>(data.constData()));

    return true;

}

bool PQCFilePluginDLLExe::readUInt64LE(QFile& file, quint64& value) {

    const QByteArray data = file.read(8);
    if(data.size() != 8)
        return false;

    value = qFromLittleEndian<quint64>(reinterpret_cast<const uchar*>(data.constData()));

    return true;

}

bool PQCFilePluginDLLExe::readBytes(QFile& file, qsizetype size, QByteArray& data) {

    if (size < 0)
        return false;

    data = file.read(size);
    return data.size() == size;

}


bool PQCFilePluginDLLExe::readUtf16String(QFile& file, QString& value) {

    QVector<ushort> chars;

    while(true) {

        quint16 ch = 0;

        if(!readUInt16LE(file, ch))
            return false;

        if(ch == 0)
            break;

        chars.append(ch);
    }

    value = QString::fromUtf16(reinterpret_cast<const char16_t*>(chars.constData()), chars.size());

    return true;

}


bool PQCFilePluginDLLExe::skipBytes(QFile& file, qsizetype size) {

    if (size < 0)
        return false;

    return file.seek(file.pos() + size);

}


quint64 PQCFilePluginDLLExe::align4(quint64 value) {
    return (value + 3) & ~quint64(3);
}

bool PQCFilePluginDLLExe::rvaToFileOffset(quint32 rva, const QList<PESection>& sections, quint64& fileOffset) {

    for(const PESection& section : sections) {

        const quint32 size = qMax(section.virtualSize, section.rawSize);

        if(rva < section.virtualAddress)
            continue;

        if(quint64(rva) >= quint64(section.virtualAddress) + size)
            continue;

        fileOffset = quint64(section.rawAddress) + quint64(rva - section.virtualAddress);

        return true;
    }

    return false;
}

bool PQCFilePluginDLLExe::readResourceDirectoryEntries(QFile& file, quint64 offset, QList<ResourceEntry>& entries) {

    entries.clear();

    if(!file.seek(offset))
        return false;

    quint32 characteristics = 0;
    quint32 timeDateStamp = 0;
    quint16 majorVersion = 0;
    quint16 minorVersion = 0;
    quint16 numberOfNamedEntries = 0;
    quint16 numberOfIdEntries = 0;

    if(!readUInt32LE(file, characteristics) || !readUInt32LE(file, timeDateStamp)        || !readUInt16LE(file, majorVersion) ||
       !readUInt16LE(file, minorVersion)    || !readUInt16LE(file, numberOfNamedEntries) || !readUInt16LE(file, numberOfIdEntries)) {
        return false;
    }

    Q_UNUSED(characteristics);
    Q_UNUSED(timeDateStamp);
    Q_UNUSED(majorVersion);
    Q_UNUSED(minorVersion);

    const quint32 count = quint32(numberOfNamedEntries) + quint32(numberOfIdEntries);

    for(quint32 i = 0; i < count; ++i) {

        ResourceEntry entry;

        if(!readUInt32LE(file, entry.name) || !readUInt32LE(file, entry.offset))
            return false;

        entries.append(entry);

    }

    return true;
}

QString PQCFilePluginDLLExe::formatVersion(quint32 ms, quint32 ls) {
    return QString("%1.%2.%3.%4").arg(ms >> 16).arg(ms & 0xffff).arg(ls >> 16).arg(ls & 0xffff);
}

QString PQCFilePluginDLLExe::formatFileFlags(quint32 fileFlags) {

    if(fileFlags == 0)
        return "NORMAL (0)";

    QStringList vals;
    if(fileFlags & VersionInfo::Debug)
        vals.append("DEBUG (0x00000001)");
    if(fileFlags & VersionInfo::Prerelease)
        vals.append("PRERELEASE (0x00000002)");
    if(fileFlags & VersionInfo::Patched)
        vals.append("PATCHED (0x00000004)");
    if(fileFlags & VersionInfo::PrivateBuild)
        vals.append("PRIVATE BUILD (0x00000008)");
    if(fileFlags & VersionInfo::InfoInferred)
        vals.append("INFO INFERRED (0x00000010)");
    if(fileFlags & 0x00000020)
        vals.append("SPECIAL BUILD (0x00000020)");

    return vals.isEmpty() ? "NONE" : vals.join(" | ");

    return vals.join(", ");
}

QString PQCFilePluginDLLExe::formatFileFlagsMask(quint32 fileFlagsMask) {

    if(fileFlagsMask == 0x0000003F)
        return "normal file flags mask (0x0000003F)";

    return QString("0x%1").arg(fileFlagsMask, 8, 16, '0');

}

QString PQCFilePluginDLLExe::formatFileOs(quint32 fileos) {

    if(fileos == VersionInfo::OsUnknown)
            return "Unknown";
    if(fileos == VersionInfo::OsDos)
            return "DOS";
    if(fileos == VersionInfo::OsOs216)
            return "OS/2 16-bit";
    if(fileos == VersionInfo::OsOs232)
            return "OS/2 32-bit";
    if(fileos == VersionInfo::OsNt)
            return "Windows NT";
    if(fileos == VersionInfo::OsDosWindows16)
            return "DOS / Windows 16-bit";
    if(fileos == VersionInfo::OsDosWindows32)
            return "DOS / Windows 32-bit";
    if(fileos == VersionInfo::OsOs216Pm16)
            return "OS/2 16-bit / Presentation Manager";
    if(fileos == VersionInfo::OsOs232Pm32)
            return "OS/2 32-bit / Presentation Manager";
    if(fileos == VersionInfo::OsNtWindows32)
            return "Windows NT / Windows 32-bit";

    return QString("Unknown (0x%1)").arg(fileos, 8, 16, '0');

}

QString PQCFilePluginDLLExe::formatFileType(quint32 filetype) {

    if(filetype == VersionInfo::TypeUnknown)
        return "Unknown";
    if(filetype == VersionInfo::TypeApp)
        return "Application";
    if(filetype == VersionInfo::TypeDll)
        return "DLL";
    if(filetype == VersionInfo::TypeDriver)
        return "Driver";
    if(filetype == VersionInfo::TypeFont)
        return "Font";
    if(filetype == VersionInfo::TypeVxd)
        return "VxD";
    if(filetype == VersionInfo::TypeStaticLib)
        return "Static library";

    return QString("Unknown (0x%1)").arg(filetype, 8, 16, QLatin1Char('0'));

}

QString PQCFilePluginDLLExe::formatFileSubtype(quint32 type, quint32 subtype) {

    if(subtype == VersionInfo::SubtypeUnknown)
        return "Unknown";

    if(type == VersionInfo::TypeDriver) {

        if(subtype == VersionInfo::DriverPrinter)
            return "Printer";
        if(subtype == VersionInfo::DriverComm)
            return "Communications";
        if(subtype == VersionInfo::DriverLanguage)
            return "Language";
        if(subtype == VersionInfo::DriverDisplay)
            return "Display";
        if(subtype == VersionInfo::DriverMouse)
            return "Mouse";
        if(subtype == VersionInfo::DriverNetwork)
            return "Network";
        if(subtype == VersionInfo::DriverSystem)
            return "System";
        if(subtype == VersionInfo::DriverInstallable)
            return "Installable";
        if(subtype == VersionInfo::DriverSound)
            return "Sound";
        if(subtype == VersionInfo::DriverKeyboard)
            return "Keyboard";
        if(subtype == VersionInfo::DriverVersionedPrinter)
            return "Versioned printer";

    } else if(type == VersionInfo::TypeFont) {

        if(subtype == VersionInfo::FontRaster)
            return "Raster";
        if(subtype == VersionInfo::FontVector)
            return "Vector";
        if(subtype == VersionInfo::FontTrueType)
            return "TrueType";

    }

    return QString("Unknown (0x%1)").arg(subtype, 8, 16, QLatin1Char('0'));

}

QString PQCFilePluginDLLExe::formatFileDate(quint32 filedateMS, quint32 filedateLS) {

    if(filedateMS == 0 && filedateLS == 0)
        return "Not specified";

    const quint64 fileDate = (quint64(filedateMS) << 32) | quint64(filedateLS);

    return QDateTime::fromSecsSinceEpoch(fileDate).toString();

}

QString PQCFilePluginDLLExe::formatStructureVersion(quint32 strucver) {
    return QString("%1.%2").arg(strucver >> 16).arg(strucver & 0xffff);
}

QString PQCFilePluginDLLExe::convertLanguageCodeToString(QString code) {

    qDebug() << "args: code =" << code;

    const QLocale locale(code);

    const QString l = QLocale::languageToString(locale.language());
    const QString t = QLocale::territoryToString(locale.territory());

    return t.isEmpty() ? l : QString("%1 (%2)").arg(l, t);

}

QString PQCFilePluginDLLExe::formatLanguageName(quint16 langId) {
    const QString code = langId2Code.value(langId, QString("Unknown / custom language (LANGID 0x%1)").arg(langId, 4, 16, '0'));
    if(code.startsWith(":"))
        return code.sliced(1);
    return convertLanguageCodeToString(code);
}

QString PQCFilePluginDLLExe::formatCodePage(quint16 codepage) {

    if(codepage == VersionInfo::CodePageUtf16Le)
        return "UTF-16LE (1200)";
    if(codepage == VersionInfo::CodePageUtf16Be)
        return "UTF-16BE (1201)";
    if(codepage == VersionInfo::CodePageUtf8)
        return "UTF-8 (65001)";
    if(codepage == 1250 || codepage == 1251 || codepage == 1252 || codepage == 1253 || codepage == 1254 ||
       codepage == 1255 || codepage == 1256 || codepage == 1257 || codepage == 1258)
        return QString("Windows-%1 (%1)").arg(codepage);
    if(codepage == 932)
        return "Shift-JIS (932)";
    if(codepage == 936)
        return "GBK (936)";
    if(codepage == 949)
        return "KS C 5601 (949)";
    if(codepage == 950)
        return "Big5 (950)";

    return QString("Code page %1").arg(codepage);

}
