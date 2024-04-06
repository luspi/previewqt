function(composeDesktopFile)

    set(fname "org.previewqt.PreviewQtRemote.desktop")

    file(WRITE  "${fname}" "[Desktop Entry]\n")
    file(APPEND "${fname}" "Name=PreviewQtRemote\n")
    file(APPEND "${fname}" "GenericName=Remote control for Previewer\n")
    file(APPEND "${fname}" "Comment=Remote control for previewing all kinds of files\n")
    file(APPEND "${fname}" "Exec=previewqtremote %f\n")
    file(APPEND "${fname}" "Icon=org.previewqt.PreviewQtRemote\n")
    file(APPEND "${fname}" "Type=Application\n")
    file(APPEND "${fname}" "Terminal=false\n")
    file(APPEND "${fname}" "Categories=Graphics;Viewer;\n")

endfunction()

