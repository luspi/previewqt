function(composeDesktopFile)

    set(fname "org.photoqt.PreviewQt.desktop")

    file(WRITE  "${fname}" "[Desktop Entry]\n")
    file(APPEND "${fname}" "Name=PreviewQt\n")
    file(APPEND "${fname}" "GenericName=Previewer\n")
    file(APPEND "${fname}" "Comment=Preview all kinds of files\n")
    file(APPEND "${fname}" "Exec=previewqt %f\n")
    file(APPEND "${fname}" "Icon=org.photoqt.PreviewQt\n")
    file(APPEND "${fname}" "Type=Application\n")
    file(APPEND "${fname}" "Terminal=false\n")
    file(APPEND "${fname}" "Categories=Graphics;Viewer;\n")

    # add the mimetypes
    set(MIMETYPE "image/avif-sequence;application/x-fpt;image/bmp;image/x-ms-bmp;image/bpg;")
    set(MIMETYPE "${MIMETYPE}image/x-canon-crw;image/x-canon-cr2;image/x-win-bitmap;application/dicom;image/dicom-rle;")
    set(MIMETYPE "${MIMETYPE}image/vnd.djvu;image/x-dpx;application/postscript;application/eps;application/x-eps;")
    set(MIMETYPE "${MIMETYPE}image/eps;image/x-eps;image/x-exr;image/fits;image/avif;")
    set(MIMETYPE "${MIMETYPE}image/gif;image/heic;image/heif;image/vnd.microsoft.icon;image/x-icon;")
    set(MIMETYPE "${MIMETYPE}application/x-pnf;video/x-jng;image/jpeg;image/jp2;image/jpx;")
    set(MIMETYPE "${MIMETYPE}image/jpm;image/jxl;application/x-krita;image/x-miff;")
    set(MIMETYPE "${MIMETYPE}image/x-mvg;application/mxf;image/openraster;image/x-olympus-orf;font/opentype;")
    set(MIMETYPE "${MIMETYPE}application/vnd.ms-opentype;image/x-portable-arbitrarymap;image/x-portable-pixmap;image/x-portable-anymap;image/vnd.zbrush.pcx;")
    set(MIMETYPE "${MIMETYPE}image/x-pcx;application/pdf;application/x-pdf;application/x-bzpdf;application/x-gzpdf;")
    set(MIMETYPE "${MIMETYPE}image/x-pentax-pef;image/x-portable-greymap;image/x-xpmi;image/png;image/vnd.adobe.photoshop;")
    set(MIMETYPE "${MIMETYPE}image/tiff;image/sgi;application/vnd.rn-realmedia;image/svg+xml;image/x-targa;")
    set(MIMETYPE "${MIMETYPE}image/x-tga;image/tiff-fx;font/sfnt;image/vnd.wap.wbmp;video/webm;")
    set(MIMETYPE "${MIMETYPE}image/webp;image/x-xbitmap;image/x-xbm;image/x-xcf;")
    set(MIMETYPE "${MIMETYPE}image/x-xpixmap;")

    file(APPEND "${fname}" "MimeType=${MIMETYPE}")

endfunction()

