
SET(previewqt_SRC cplusplus/main.cpp

                  cplusplus/singletons/pqc_cache.cpp
                  cplusplus/singletons/pqc_settings.cpp
                  cplusplus/singletons/pqc_textprocessing.cpp

                  cplusplus/singletons/pqc_scriptsconfig.cpp
                  cplusplus/singletons/pqc_scriptsfilespaths.cpp
                  cplusplus/singletons/pqc_scriptsimages.cpp
                  cplusplus/singletons/pqc_scriptsother.cpp
                  cplusplus/singletons/pqc_scriptsexternaltools.cpp

                  cplusplus/files/plugins/pqc_fileplugin.cpp
                  cplusplus/files/plugins/pqc_fileplugin_audio.cpp
                  cplusplus/files/plugins/pqc_fileplugin_devil.cpp
                  cplusplus/files/plugins/pqc_fileplugin_ebook.cpp
                  cplusplus/files/plugins/pqc_fileplugin_libarchive.cpp
                  cplusplus/files/plugins/pqc_fileplugin_libraw.cpp
                  cplusplus/files/plugins/pqc_fileplugin_libsai.cpp
                  cplusplus/files/plugins/pqc_fileplugin_libvips.cpp
                  cplusplus/files/plugins/pqc_fileplugin_magick.cpp
                  cplusplus/files/plugins/pqc_fileplugin_pdf.cpp
                  cplusplus/files/plugins/pqc_fileplugin_qt.cpp
                  cplusplus/files/plugins/pqc_fileplugin_resvg.cpp
                  cplusplus/files/plugins/pqc_fileplugin_text.cpp
                  cplusplus/files/plugins/pqc_fileplugin_video.cpp

                  cplusplus/files/pqc_filehandler.cpp
                  cplusplus/files/pqc_imageproviderfull.cpp
                  cplusplus/files/pqc_imageprovidersvg.cpp

                  cplusplus/other/pqc_singleinstance.cpp
                  cplusplus/other/pqc_specialactions.cpp
                  cplusplus/other/pqc_helper.cpp

                  cplusplus/header/pqc_cache.h
                  cplusplus/header/pqc_constants.h
                  cplusplus/header/pqc_messagehandler.h
                  cplusplus/header/pqc_notify.h
                  cplusplus/header/pqc_settings.h
                  cplusplus/header/pqc_settingscpp.h
                  cplusplus/header/pqc_singleinstance.h
                  cplusplus/header/pqc_specialactions.h
                  cplusplus/header/pqc_textprocessing.h
                  cplusplus/header/pqc_configfiles.h

                  cplusplus/header/pqc_scriptsconfig.h
                  cplusplus/header/pqc_scriptsconfig_qml.h
                  cplusplus/header/pqc_scriptsfilespaths.h
                  cplusplus/header/pqc_scriptsfilespaths_qml.h
                  cplusplus/header/pqc_scriptsimages.h
                  cplusplus/header/pqc_scriptsimages_qml.h
                  cplusplus/header/pqc_scriptsother.h
                  cplusplus/header/pqc_scriptsother_qml.h
                  cplusplus/header/pqc_scriptsexternaltools.h
                  cplusplus/header/pqc_scriptsexternaltools_qml.h
                  cplusplus/header/pqc_filehandler.h
                  cplusplus/header/pqc_filehandler_qml.h

                  cplusplus/header/fileplugins/pqc_fileplugin.h
                  cplusplus/header/fileplugins/pqc_fileplugin_audio.h
                  cplusplus/header/fileplugins/pqc_fileplugin_devil.h
                  cplusplus/header/fileplugins/pqc_fileplugin_ebook.h
                  cplusplus/header/fileplugins/pqc_fileplugin_libarchive.h
                  cplusplus/header/fileplugins/pqc_fileplugin_libraw.h
                  cplusplus/header/fileplugins/pqc_fileplugin_libsai.h
                  cplusplus/header/fileplugins/pqc_fileplugin_libvips.h
                  cplusplus/header/fileplugins/pqc_fileplugin_magick.h
                  cplusplus/header/fileplugins/pqc_fileplugin_pdf.h
                  cplusplus/header/fileplugins/pqc_fileplugin_qt.h
                  cplusplus/header/fileplugins/pqc_fileplugin_resvg.h
                  cplusplus/header/fileplugins/pqc_fileplugin_text.h
                  cplusplus/header/fileplugins/pqc_fileplugin_video.h
)

if(WITH_LIBMPV)
    set(previewqt_SRC ${previewqt_SRC} cplusplus/header/pqc_mpvobject.h
                                       cplusplus/other/pqc_mpvobject.cpp)
endif()
