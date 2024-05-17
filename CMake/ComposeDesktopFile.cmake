function(composeDesktopFile)

    set(fname "org.previewqt.PreviewQt.desktop")

    file(WRITE  "${fname}" "[Desktop Entry]\n")
    file(APPEND "${fname}" "Name=PreviewQt\n")
    file(APPEND "${fname}" "GenericName=Previewer\n")
    file(APPEND "${fname}" "Comment=Preview all kinds of files\n")
    file(APPEND "${fname}" "Exec=previewqt %f\n")
    file(APPEND "${fname}" "Icon=org.previewqt.PreviewQt\n")
    file(APPEND "${fname}" "Type=Application\n")
    file(APPEND "${fname}" "Terminal=false\n")
    file(APPEND "${fname}" "Categories=Graphics;Viewer;\n")

    # add the mimetypes
    set(MIMETYPE "application/x-navi-animation;video/x-ms-asf;application/vnd.ms-asf;image/avif;image/avif-sequence;")
    set(MIMETYPE "${MIMETYPE}application/x-fpt;image/bmp;image/x-ms-bmp;image/bpg;image/x-canon-crw;")
    set(MIMETYPE "${MIMETYPE}image/x-canon-cr2;image/x-win-bitmap;application/dicom;image/dicom-rle;image/vnd.djvu;")
    set(MIMETYPE "${MIMETYPE}image/x-dpx;application/epub+zip;image/x-exr;image/fits;image/gif;")
    set(MIMETYPE "${MIMETYPE}image/heic;image/heif;image/vnd.microsoft.icon;image/x-icon;application/x-pnf;")
    set(MIMETYPE "${MIMETYPE}video/x-jng;image/jpeg;image/jp2;image/jpx;image/jpm;")
    set(MIMETYPE "${MIMETYPE}image/jxl;application/x-krita;image/x-miff;video/x-mng;image/x-mvg;")
    set(MIMETYPE "${MIMETYPE}image/openraster;image/x-olympus-orf;font/opentype;application/vnd.ms-opentype;image/x-portable-arbitrarymap;")
    set(MIMETYPE "${MIMETYPE}image/x-portable-pixmap;image/x-portable-anymap;image/vnd.zbrush.pcx;image/x-pcx;image/x-pentax-pef;")
    set(MIMETYPE "${MIMETYPE}image/x-portable-greymap;image/x-xpmi;image/png;image/vnd.adobe.photoshop;image/tiff;")
    set(MIMETYPE "${MIMETYPE}image/sgi;image/svg+xml;image/x-targa;image/x-tga;image/tiff-fx;")
    set(MIMETYPE "${MIMETYPE}font/sfnt;image/vnd.wap.wbmp;image/webp;image/x-xbitmap;image/x-xbm;")
    set(MIMETYPE "${MIMETYPE}image/x-xcf;image/x-xpixmap;")

    file(APPEND "${fname}" "MimeType=${MIMETYPE}")

endfunction()

