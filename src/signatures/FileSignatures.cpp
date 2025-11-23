#include "../include/FileSignatures.h"
#include "SignatureDatabase.h"
#include "SignatureLoader.h"
#include "../utils/Logger.h"
#include <algorithm>
#include <filesystem>

namespace FileRecovery {

class FileSignatures::Impl {
public:
    SignatureDatabase database;
};

FileSignatures& FileSignatures::getInstance() {
    static FileSignatures instance;
    return instance;
}

FileSignatures::FileSignatures() : pImpl(std::make_unique<Impl>()) {
    initializeSignatures();
}

void FileSignatures::initializeSignatures() {
    // Essayer de charger depuis fichier externe d'abord
    if (loadFromExternalFile("signatures.txt")) {
        Logger::getInstance().log(LogLevel::INFO, "Loaded signatures from external file");
    } else {
        // Fallback: signatures codées en dur
        Logger::getInstance().log(LogLevel::WARNING, "Could not load external signatures, using built-in ones");
        addCommonSignatures();
        addImageSignatures();
        addVideoSignatures();
        addAudioSignatures();
        addDocumentSignatures();
        addArchiveSignatures();
        addGameSignatures();       // Jeux vidéo (GVAS, etc.)
        addExecutableSignatures(); // EXE, DLL, ELF, APK
        addDatabaseSignatures();   // SQLite, DB, MDB, etc.
        add3DSignatures();         // OBJ, STL, FBX, BLEND
        addCryptoSignatures();     // Wallets crypto
    }

    Logger::getInstance().log(LogLevel::INFO, "Initialized " + std::to_string(getSignatureCount()) + " file signatures");
}

bool FileSignatures::loadFromExternalFile(const std::string& filename) {
    // Chercher dans plusieurs emplacements
    std::vector<std::string> searchPaths = {
        filename,                           // Chemin relatif
        "./" + filename,                    // Dossier courant
        "../" + filename,                   // Dossier parent
        "config/" + filename,               // Sous-dossier config
    };
    
    SignatureLoader loader;
    
    for (const auto& path : searchPaths) {
        std::error_code ec;
        if (std::filesystem::exists(path, ec)) {
            if (loader.loadFromFile(path)) {
                auto signatures = loader.getSignatures();
                for (const auto& sig : signatures) {
                    pImpl->database.addSignature(sig);
                }
                Logger::getInstance().log(LogLevel::INFO, "Loaded " + std::to_string(signatures.size()) + 
                                         " signatures from: " + path);
                return true;
            } else {
                Logger::getInstance().log(LogLevel::WARNING, "Failed to parse: " + path + " - " + loader.getLastError());
            }
        }
    }
    
    return false;
}

std::optional<FileSignature> FileSignatures::detectFileType(const ByteArray& data) const {
    return pImpl->database.detectSignature(data);
}

std::optional<FileSignature> FileSignatures::detectFileTypeByExtension(const std::string& extension) const {
    return pImpl->database.getSignatureByExtension(extension);
}

bool FileSignatures::validateSignature(const ByteArray& data, const FileSignature& signature) const {
    if (data.size() < signature.header.size()) {
        return false;
    }
    
    return std::equal(signature.header.begin(), signature.header.end(), 
                     data.begin() + signature.headerOffset);
}

bool FileSignatures::hasValidHeader(const ByteArray& data, const std::string& extension) const {
    auto sig = detectFileTypeByExtension(extension);
    if (!sig) return false;
    
    return validateSignature(data, *sig);
}

bool FileSignatures::hasValidFooter(const ByteArray& data, const std::string& extension) const {
    auto sig = detectFileTypeByExtension(extension);
    if (!sig || sig->footer.empty()) return false;
    
    if (data.size() < sig->footer.size()) return false;
    
    size_t footerStart = data.size() - sig->footer.size();
    return std::equal(sig->footer.begin(), sig->footer.end(), data.begin() + footerStart);
}

std::vector<Offset> FileSignatures::findSignatures(const ByteArray& data, 
                                                   const std::string& extension) const {
    return pImpl->database.findSignaturesInData(data, extension);
}

void FileSignatures::addSignature(const FileSignature& signature) {
    pImpl->database.addSignature(signature);
}

std::vector<FileSignature> FileSignatures::getAllSignatures() const {
    return pImpl->database.getAllSignatures();
}

std::vector<std::string> FileSignatures::getSupportedExtensions() const {
    return pImpl->database.getSupportedExtensions();
}

size_t FileSignatures::getSignatureCount() const {
    return pImpl->database.getCount();
}

void FileSignatures::addCommonSignatures() {
    // Signatures communes
}

void FileSignatures::addImageSignatures() {
    // JPEG
    FileSignature jpeg;
    jpeg.extension = "jpg";
    jpeg.mimeType = "image/jpeg";
    jpeg.description = "JPEG Image";
    jpeg.header = {0xFF, 0xD8, 0xFF};
    jpeg.footer = {0xFF, 0xD9};
    addSignature(jpeg);

    // PNG
    FileSignature png;
    png.extension = "png";
    png.mimeType = "image/png";
    png.description = "PNG Image";
    png.header = {0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A};
    png.footer = {0x49, 0x45, 0x4E, 0x44, 0xAE, 0x42, 0x60, 0x82};
    addSignature(png);

    // GIF
    FileSignature gif;
    gif.extension = "gif";
    gif.mimeType = "image/gif";
    gif.description = "GIF Image";
    gif.header = {0x47, 0x49, 0x46, 0x38}; // GIF8
    addSignature(gif);

    // BMP
    FileSignature bmp;
    bmp.extension = "bmp";
    bmp.mimeType = "image/bmp";
    bmp.description = "Bitmap Image";
    bmp.header = {0x42, 0x4D}; // BM
    addSignature(bmp);

    // TIFF (Little Endian)
    FileSignature tiff_le;
    tiff_le.extension = "tiff";
    tiff_le.mimeType = "image/tiff";
    tiff_le.description = "TIFF Image (Little Endian)";
    tiff_le.header = {0x49, 0x49, 0x2A, 0x00};
    addSignature(tiff_le);

    // TIFF (Big Endian)
    FileSignature tiff_be;
    tiff_be.extension = "tiff";
    tiff_be.mimeType = "image/tiff";
    tiff_be.description = "TIFF Image (Big Endian)";
    tiff_be.header = {0x4D, 0x4D, 0x00, 0x2A};
    addSignature(tiff_be);

    // WebP
    FileSignature webp;
    webp.extension = "webp";
    webp.mimeType = "image/webp";
    webp.description = "WebP Image";
    webp.header = {0x52, 0x49, 0x46, 0x46}; // RIFF
    addSignature(webp);

    // ICO
    FileSignature ico;
    ico.extension = "ico";
    ico.mimeType = "image/x-icon";
    ico.description = "Icon File";
    ico.header = {0x00, 0x00, 0x01, 0x00};
    addSignature(ico);

    // PSD (Adobe Photoshop)
    FileSignature psd;
    psd.extension = "psd";
    psd.mimeType = "image/vnd.adobe.photoshop";
    psd.description = "Adobe Photoshop Document";
    psd.header = {0x38, 0x42, 0x50, 0x53}; // 8BPS
    addSignature(psd);

    // CR2 (Canon RAW)
    FileSignature cr2;
    cr2.extension = "cr2";
    cr2.mimeType = "image/x-canon-cr2";
    cr2.description = "Canon RAW Image";
    cr2.header = {0x49, 0x49, 0x2A, 0x00};
    addSignature(cr2);

    // NEF (Nikon RAW)
    FileSignature nef;
    nef.extension = "nef";
    nef.mimeType = "image/x-nikon-nef";
    nef.description = "Nikon RAW Image";
    nef.header = {0x4D, 0x4D, 0x00, 0x2A};
    addSignature(nef);

    // HEIC (Apple HEIF)
    FileSignature heic;
    heic.extension = "heic";
    heic.mimeType = "image/heic";
    heic.description = "HEIF Image Container";
    heic.header = {0x66, 0x74, 0x79, 0x70, 0x68, 0x65, 0x69, 0x63}; // ftypheic
    heic.headerOffset = 4;
    addSignature(heic);

    // SVG
    FileSignature svg;
    svg.extension = "svg";
    svg.mimeType = "image/svg+xml";
    svg.description = "Scalable Vector Graphics";
    svg.header = {0x3C, 0x3F, 0x78, 0x6D, 0x6C}; // <?xml
    addSignature(svg);
}

void FileSignatures::addVideoSignatures() {
    // MP4
    FileSignature mp4;
    mp4.extension = "mp4";
    mp4.mimeType = "video/mp4";
    mp4.description = "MP4 Video";
    mp4.header = {0x00, 0x00, 0x00, 0x18, 0x66, 0x74, 0x79, 0x70}; // ftyp
    mp4.headerOffset = 4;
    addSignature(mp4);

    // AVI
    FileSignature avi;
    avi.extension = "avi";
    avi.mimeType = "video/x-msvideo";
    avi.description = "AVI Video";
    avi.header = {0x52, 0x49, 0x46, 0x46}; // RIFF
    addSignature(avi);

    // MKV (Matroska)
    FileSignature mkv;
    mkv.extension = "mkv";
    mkv.mimeType = "video/x-matroska";
    mkv.description = "Matroska Video";
    mkv.header = {0x1A, 0x45, 0xDF, 0xA3};
    addSignature(mkv);

    // MOV (QuickTime)
    FileSignature mov;
    mov.extension = "mov";
    mov.mimeType = "video/quicktime";
    mov.description = "QuickTime Movie";
    mov.header = {0x66, 0x74, 0x79, 0x70, 0x71, 0x74}; // ftypqt
    mov.headerOffset = 4;
    addSignature(mov);

    // FLV (Flash Video)
    FileSignature flv;
    flv.extension = "flv";
    flv.mimeType = "video/x-flv";
    flv.description = "Flash Video";
    flv.header = {0x46, 0x4C, 0x56, 0x01}; // FLV
    addSignature(flv);

    // WMV (Windows Media Video)
    FileSignature wmv;
    wmv.extension = "wmv";
    wmv.mimeType = "video/x-ms-wmv";
    wmv.description = "Windows Media Video";
    wmv.header = {0x30, 0x26, 0xB2, 0x75, 0x8E, 0x66, 0xCF, 0x11};
    addSignature(wmv);

    // MPEG
    FileSignature mpeg;
    mpeg.extension = "mpeg";
    mpeg.mimeType = "video/mpeg";
    mpeg.description = "MPEG Video";
    mpeg.header = {0x00, 0x00, 0x01, 0xBA};
    addSignature(mpeg);

    // WEBM
    FileSignature webm;
    webm.extension = "webm";
    webm.mimeType = "video/webm";
    webm.description = "WebM Video";
    webm.header = {0x1A, 0x45, 0xDF, 0xA3};
    addSignature(webm);

    // M4V (iTunes Video)
    FileSignature m4v;
    m4v.extension = "m4v";
    m4v.mimeType = "video/x-m4v";
    m4v.description = "iTunes Video File";
    m4v.header = {0x66, 0x74, 0x79, 0x70, 0x4D, 0x34, 0x56}; // ftypM4V
    m4v.headerOffset = 4;
    addSignature(m4v);
}

void FileSignatures::addAudioSignatures() {
    // MP3
    FileSignature mp3;
    mp3.extension = "mp3";
    mp3.mimeType = "audio/mpeg";
    mp3.description = "MP3 Audio";
    mp3.header = {0xFF, 0xFB}; // ID3 ou frame header
    addSignature(mp3);

    // WAV
    FileSignature wav;
    wav.extension = "wav";
    wav.mimeType = "audio/wav";
    wav.description = "WAV Audio";
    wav.header = {0x52, 0x49, 0x46, 0x46}; // RIFF
    addSignature(wav);

    // FLAC
    FileSignature flac;
    flac.extension = "flac";
    flac.mimeType = "audio/flac";
    flac.description = "Free Lossless Audio Codec";
    flac.header = {0x66, 0x4C, 0x61, 0x43}; // fLaC
    addSignature(flac);

    // OGG
    FileSignature ogg;
    ogg.extension = "ogg";
    ogg.mimeType = "audio/ogg";
    ogg.description = "Ogg Vorbis Audio";
    ogg.header = {0x4F, 0x67, 0x67, 0x53}; // OggS
    addSignature(ogg);

    // M4A (AAC Audio)
    FileSignature m4a;
    m4a.extension = "m4a";
    m4a.mimeType = "audio/mp4";
    m4a.description = "MPEG-4 Audio";
    m4a.header = {0x66, 0x74, 0x79, 0x70, 0x4D, 0x34, 0x41}; // ftypM4A
    m4a.headerOffset = 4;
    addSignature(m4a);

    // AAC
    FileSignature aac;
    aac.extension = "aac";
    aac.mimeType = "audio/aac";
    aac.description = "Advanced Audio Coding";
    aac.header = {0xFF, 0xF1};
    addSignature(aac);

    // WMA (Windows Media Audio)
    FileSignature wma;
    wma.extension = "wma";
    wma.mimeType = "audio/x-ms-wma";
    wma.description = "Windows Media Audio";
    wma.header = {0x30, 0x26, 0xB2, 0x75, 0x8E, 0x66, 0xCF, 0x11};
    addSignature(wma);

    // MIDI
    FileSignature midi;
    midi.extension = "mid";
    midi.mimeType = "audio/midi";
    midi.description = "MIDI Audio";
    midi.header = {0x4D, 0x54, 0x68, 0x64}; // MThd
    addSignature(midi);
}

void FileSignatures::addDocumentSignatures() {
    // PDF
    FileSignature pdf;
    pdf.extension = "pdf";
    pdf.mimeType = "application/pdf";
    pdf.description = "PDF Document";
    pdf.header = {0x25, 0x50, 0x44, 0x46}; // %PDF
    pdf.footer = {0x25, 0x25, 0x45, 0x4F, 0x46}; // %%EOF
    addSignature(pdf);

    // DOC (Office 97-2003)
    FileSignature doc;
    doc.extension = "doc";
    doc.mimeType = "application/msword";
    doc.description = "Microsoft Word Document";
    doc.header = {0xD0, 0xCF, 0x11, 0xE0, 0xA1, 0xB1, 0x1A, 0xE1};
    addSignature(doc);

    // DOCX (Office 2007+)
    FileSignature docx;
    docx.extension = "docx";
    docx.mimeType = "application/vnd.openxmlformats-officedocument.wordprocessingml.document";
    docx.description = "Microsoft Word Document (XML)";
    docx.header = {0x50, 0x4B, 0x03, 0x04}; // PK (ZIP)
    addSignature(docx);

    // XLSX (Excel 2007+)
    FileSignature xlsx;
    xlsx.extension = "xlsx";
    xlsx.mimeType = "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet";
    xlsx.description = "Microsoft Excel Spreadsheet (XML)";
    xlsx.header = {0x50, 0x4B, 0x03, 0x04}; // PK (ZIP)
    addSignature(xlsx);

    // XLS (Excel 97-2003)
    FileSignature xls;
    xls.extension = "xls";
    xls.mimeType = "application/vnd.ms-excel";
    xls.description = "Microsoft Excel Spreadsheet";
    xls.header = {0xD0, 0xCF, 0x11, 0xE0, 0xA1, 0xB1, 0x1A, 0xE1};
    addSignature(xls);

    // PPTX (PowerPoint 2007+)
    FileSignature pptx;
    pptx.extension = "pptx";
    pptx.mimeType = "application/vnd.openxmlformats-officedocument.presentationml.presentation";
    pptx.description = "Microsoft PowerPoint Presentation (XML)";
    pptx.header = {0x50, 0x4B, 0x03, 0x04}; // PK (ZIP)
    addSignature(pptx);

    // PPT (PowerPoint 97-2003)
    FileSignature ppt;
    ppt.extension = "ppt";
    ppt.mimeType = "application/vnd.ms-powerpoint";
    ppt.description = "Microsoft PowerPoint Presentation";
    ppt.header = {0xD0, 0xCF, 0x11, 0xE0, 0xA1, 0xB1, 0x1A, 0xE1};
    addSignature(ppt);

    // RTF
    FileSignature rtf;
    rtf.extension = "rtf";
    rtf.mimeType = "application/rtf";
    rtf.description = "Rich Text Format";
    rtf.header = {0x7B, 0x5C, 0x72, 0x74, 0x66, 0x31}; // {\rtf1
    addSignature(rtf);

    // EPUB
    FileSignature epub;
    epub.extension = "epub";
    epub.mimeType = "application/epub+zip";
    epub.description = "Electronic Publication";
    epub.header = {0x50, 0x4B, 0x03, 0x04}; // PK (ZIP)
    addSignature(epub);

    // ODT (OpenDocument Text)
    FileSignature odt;
    odt.extension = "odt";
    odt.mimeType = "application/vnd.oasis.opendocument.text";
    odt.description = "OpenDocument Text";
    odt.header = {0x50, 0x4B, 0x03, 0x04}; // PK (ZIP)
    addSignature(odt);
}

void FileSignatures::addArchiveSignatures() {
    // ZIP
    FileSignature zip;
    zip.extension = "zip";
    zip.mimeType = "application/zip";
    zip.description = "ZIP Archive";
    zip.header = {0x50, 0x4B, 0x03, 0x04}; // PK
    addSignature(zip);

    // RAR
    FileSignature rar;
    rar.extension = "rar";
    rar.mimeType = "application/x-rar-compressed";
    rar.description = "RAR Archive";
    rar.header = {0x52, 0x61, 0x72, 0x21, 0x1A, 0x07}; // Rar!
    addSignature(rar);

    // 7Z
    FileSignature sevenZ;
    sevenZ.extension = "7z";
    sevenZ.mimeType = "application/x-7z-compressed";
    sevenZ.description = "7-Zip Archive";
    sevenZ.header = {0x37, 0x7A, 0xBC, 0xAF, 0x27, 0x1C}; // 7z
    addSignature(sevenZ);

    // TAR
    FileSignature tar;
    tar.extension = "tar";
    tar.mimeType = "application/x-tar";
    tar.description = "Tar Archive";
    tar.header = {0x75, 0x73, 0x74, 0x61, 0x72}; // ustar
    tar.headerOffset = 257;
    addSignature(tar);

    // GZ (GZIP)
    FileSignature gz;
    gz.extension = "gz";
    gz.mimeType = "application/gzip";
    gz.description = "GZIP Compressed Archive";
    gz.header = {0x1F, 0x8B, 0x08};
    addSignature(gz);

    // BZ2 (BZIP2)
    FileSignature bz2;
    bz2.extension = "bz2";
    bz2.mimeType = "application/x-bzip2";
    bz2.description = "BZIP2 Compressed Archive";
    bz2.header = {0x42, 0x5A, 0x68}; // BZh
    addSignature(bz2);

    // ISO
    FileSignature iso;
    iso.extension = "iso";
    iso.mimeType = "application/x-iso9660-image";
    iso.description = "ISO Disc Image";
    iso.header = {0x43, 0x44, 0x30, 0x30, 0x31}; // CD001
    iso.headerOffset = 0x8001;
    addSignature(iso);
}

void FileSignatures::addGameSignatures() {
    // GVAS (Unreal Engine Save File)
    FileSignature gvas;
    gvas.extension = "sav";
    gvas.mimeType = "application/x-unreal-savegame";
    gvas.description = "Unreal Engine Game Save (GVAS)";
    gvas.header = {0x47, 0x56, 0x41, 0x53}; // "GVAS" en ASCII
    addSignature(gvas);

    Logger::getInstance().log(LogLevel::INFO, "Added GVAS signature for Unreal Engine saves (Octopath Traveler 2, etc.)");
}

void FileSignatures::addExecutableSignatures() {
    // EXE (Windows Executable)
    FileSignature exe;
    exe.extension = "exe";
    exe.mimeType = "application/x-msdownload";
    exe.description = "Windows Executable";
    exe.header = {0x4D, 0x5A}; // MZ
    addSignature(exe);

    // DLL (Windows Dynamic Link Library)
    FileSignature dll;
    dll.extension = "dll";
    dll.mimeType = "application/x-msdownload";
    dll.description = "Windows Dynamic Link Library";
    dll.header = {0x4D, 0x5A}; // MZ
    addSignature(dll);

    // ELF (Linux Executable)
    FileSignature elf;
    elf.extension = "elf";
    elf.mimeType = "application/x-executable";
    elf.description = "Linux/Unix Executable";
    elf.header = {0x7F, 0x45, 0x4C, 0x46}; // .ELF
    addSignature(elf);

    // APK (Android Package)
    FileSignature apk;
    apk.extension = "apk";
    apk.mimeType = "application/vnd.android.package-archive";
    apk.description = "Android Application Package";
    apk.header = {0x50, 0x4B, 0x03, 0x04}; // PK (ZIP)
    addSignature(apk);

    // MSI (Windows Installer)
    FileSignature msi;
    msi.extension = "msi";
    msi.mimeType = "application/x-msi";
    msi.description = "Windows Installer Package";
    msi.header = {0xD0, 0xCF, 0x11, 0xE0, 0xA1, 0xB1, 0x1A, 0xE1};
    addSignature(msi);

    // DMG (macOS Disk Image)
    FileSignature dmg;
    dmg.extension = "dmg";
    dmg.mimeType = "application/x-apple-diskimage";
    dmg.description = "macOS Disk Image";
    dmg.header = {0x78, 0x01, 0x73, 0x0D, 0x62, 0x62, 0x60};
    addSignature(dmg);
}

void FileSignatures::addDatabaseSignatures() {
    // SQLite
    FileSignature sqlite;
    sqlite.extension = "db";
    sqlite.mimeType = "application/x-sqlite3";
    sqlite.description = "SQLite Database";
    sqlite.header = {0x53, 0x51, 0x4C, 0x69, 0x74, 0x65, 0x20, 0x66, 0x6F, 0x72, 0x6D, 0x61, 0x74, 0x20, 0x33}; // SQLite format 3
    addSignature(sqlite);

    // MDB (Microsoft Access Database)
    FileSignature mdb;
    mdb.extension = "mdb";
    mdb.mimeType = "application/x-msaccess";
    mdb.description = "Microsoft Access Database";
    mdb.header = {0x00, 0x01, 0x00, 0x00, 0x53, 0x74, 0x61, 0x6E, 0x64, 0x61, 0x72, 0x64, 0x20, 0x4A, 0x65, 0x74}; // Standard Jet
    addSignature(mdb);

    // ACCDB (Access 2007+)
    FileSignature accdb;
    accdb.extension = "accdb";
    accdb.mimeType = "application/x-msaccess";
    accdb.description = "Microsoft Access Database (2007+)";
    accdb.header = {0x00, 0x01, 0x00, 0x00, 0x53, 0x74, 0x61, 0x6E, 0x64, 0x61, 0x72, 0x64, 0x20, 0x41, 0x43, 0x45}; // Standard ACE
    addSignature(accdb);

    // PST (Outlook Email Database)
    FileSignature pst;
    pst.extension = "pst";
    pst.mimeType = "application/vnd.ms-outlook";
    pst.description = "Outlook Personal Storage";
    pst.header = {0x21, 0x42, 0x44, 0x4E}; // !BDN
    addSignature(pst);

    // OST (Outlook Offline Storage)
    FileSignature ost;
    ost.extension = "ost";
    ost.mimeType = "application/vnd.ms-outlook";
    ost.description = "Outlook Offline Storage";
    ost.header = {0x21, 0x42, 0x44, 0x4E}; // !BDN
    addSignature(ost);
}

void FileSignatures::add3DSignatures() {
    // OBJ (Wavefront 3D Object)
    FileSignature obj;
    obj.extension = "obj";
    obj.mimeType = "model/obj";
    obj.description = "Wavefront 3D Object";
    obj.header = {0x23, 0x20}; // "# " (comment)
    addSignature(obj);

    // STL (Stereolithography - Binary)
    FileSignature stl;
    stl.extension = "stl";
    stl.mimeType = "model/stl";
    stl.description = "Stereolithography 3D Model";
    stl.header = {0x73, 0x6F, 0x6C, 0x69, 0x64}; // "solid" (ASCII) ou binary
    addSignature(stl);

    // FBX (Autodesk Filmbox)
    FileSignature fbx;
    fbx.extension = "fbx";
    fbx.mimeType = "application/x-fbx";
    fbx.description = "Autodesk Filmbox 3D";
    fbx.header = {0x4B, 0x61, 0x79, 0x64, 0x61, 0x72, 0x61, 0x20, 0x46, 0x42, 0x58, 0x20, 0x42, 0x69, 0x6E, 0x61, 0x72, 0x79}; // Kaydara FBX Binary
    addSignature(fbx);

    // BLEND (Blender)
    FileSignature blend;
    blend.extension = "blend";
    blend.mimeType = "application/x-blender";
    blend.description = "Blender 3D Project";
    blend.header = {0x42, 0x4C, 0x45, 0x4E, 0x44, 0x45, 0x52}; // BLENDER
    addSignature(blend);

    // 3DS (3D Studio)
    FileSignature _3ds;
    _3ds.extension = "3ds";
    _3ds.mimeType = "image/x-3ds";
    _3ds.description = "3D Studio Model";
    _3ds.header = {0x4D, 0x4D}; // MM
    addSignature(_3ds);

    // DXF (AutoCAD Drawing Exchange Format)
    FileSignature dxf;
    dxf.extension = "dxf";
    dxf.mimeType = "image/vnd.dxf";
    dxf.description = "AutoCAD Drawing Exchange Format";
    dxf.header = {0x30, 0x0D, 0x0A, 0x53, 0x45, 0x43, 0x54, 0x49, 0x4F, 0x4E}; // 0..SECTION
    addSignature(dxf);

    // DWG (AutoCAD Drawing)
    FileSignature dwg;
    dwg.extension = "dwg";
    dwg.mimeType = "image/vnd.dwg";
    dwg.description = "AutoCAD Drawing";
    dwg.header = {0x41, 0x43, 0x31, 0x30}; // AC10
    addSignature(dwg);
}

void FileSignatures::addCryptoSignatures() {
    // Bitcoin Core Wallet
    FileSignature wallet_dat;
    wallet_dat.extension = "dat";
    wallet_dat.mimeType = "application/x-bitcoin-wallet";
    wallet_dat.description = "Bitcoin Core Wallet";
    wallet_dat.header = {0x00, 0x00, 0x00, 0x00, 0x62, 0x31, 0x05, 0x00}; // Berkeley DB magic
    addSignature(wallet_dat);

    // Ethereum Keystore (JSON)
    FileSignature keystore;
    keystore.extension = "json";
    keystore.mimeType = "application/json";
    keystore.description = "Ethereum Keystore";
    keystore.header = {0x7B, 0x22}; // {" (JSON start)
    addSignature(keystore);

    // PEM (Privacy Enhanced Mail - used for crypto keys)
    FileSignature pem;
    pem.extension = "pem";
    pem.mimeType = "application/x-pem-file";
    pem.description = "PEM Certificate/Key";
    pem.header = {0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x42, 0x45, 0x47, 0x49, 0x4E}; // -----BEGIN
    addSignature(pem);

    // P12/PFX (PKCS#12 Certificate)
    FileSignature p12;
    p12.extension = "p12";
    p12.mimeType = "application/x-pkcs12";
    p12.description = "PKCS#12 Certificate";
    p12.header = {0x30, 0x82};
    addSignature(p12);
}

} // namespace FileRecovery
