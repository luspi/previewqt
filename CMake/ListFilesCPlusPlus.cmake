
SET(previewqt_SRC "cplusplus/main.cpp"

                  "cplusplus/singletons/pqc_cache.cpp"
                  "cplusplus/singletons/pqc_fileformats.cpp"
                  "cplusplus/singletons/pqc_settings.cpp"
                  "cplusplus/singletons/pqc_textprocessing.cpp"

                  "cplusplus/singletons/pqc_scriptsconfig.cpp"
                  "cplusplus/singletons/pqc_scriptsfilespaths.cpp"
                  "cplusplus/singletons/pqc_scriptsimages.cpp"
                  "cplusplus/singletons/pqc_scriptsother.cpp"
                  "cplusplus/singletons/pqc_scriptsexternaltools.cpp"

                  "cplusplus/files/pqc_loadfile.cpp"
                  "cplusplus/files/pqc_providerfull.cpp"
                  "cplusplus/files/pqc_providersvg.cpp"

                  "cplusplus/files/plugins/pqc_loadfile_archive.cpp"
                  "cplusplus/files/plugins/pqc_loadfile_devil.cpp"
                  "cplusplus/files/plugins/pqc_loadfile_freeimage.cpp"
                  "cplusplus/files/plugins/pqc_loadfile_libvips.cpp"
                  "cplusplus/files/plugins/pqc_loadfile_magick.cpp"
                  "cplusplus/files/plugins/pqc_loadfile_poppler.cpp"
                  "cplusplus/files/plugins/pqc_loadfile_qt.cpp"
                  "cplusplus/files/plugins/pqc_loadfile_qtpdf.cpp"
                  "cplusplus/files/plugins/pqc_loadfile_raw.cpp"
                  "cplusplus/files/plugins/pqc_loadfile_resvg.cpp"
                  "cplusplus/files/plugins/pqc_loadfile_video.cpp"
                  "cplusplus/files/plugins/pqc_loadfile_xcf.cpp"

                  "cplusplus/other/pqc_photosphere.cpp"
                  "cplusplus/other/pqc_photosphereitem.cpp"
                  "cplusplus/other/pqc_photosphererenderer.cpp"
                  "cplusplus/other/pqc_singleinstance.cpp"
                  "cplusplus/other/pqc_specialactions.cpp"

                  "cplusplus/header/pqc_cache.h"
                  "cplusplus/header/pqc_constants.h"
                  "cplusplus/header/pqc_fileformats.h"
                  "cplusplus/header/pqc_messagehandler.h"
                  "cplusplus/header/pqc_notify.h"
                  "cplusplus/header/pqc_photosphere.h"
                  "cplusplus/header/pqc_photosphereitem.h"
                  "cplusplus/header/pqc_photosphererenderer.h"
                  "cplusplus/header/pqc_settings.h"
                  "cplusplus/header/pqc_settingscpp.h"
                  "cplusplus/header/pqc_singleinstance.h"
                  "cplusplus/header/pqc_specialactions.h"
                  "cplusplus/header/pqc_textprocessing.h"

                  "cplusplus/header/pqc_scriptsconfig.h"
                  "cplusplus/header/pqc_scriptsconfig_qml.h"
                  "cplusplus/header/pqc_scriptsfilespaths.h"
                  "cplusplus/header/pqc_scriptsfilespaths_qml.h"
                  "cplusplus/header/pqc_scriptsimages.h"
                  "cplusplus/header/pqc_scriptsimages_qml.h"
                  "cplusplus/header/pqc_scriptsother.h"
                  "cplusplus/header/pqc_scriptsother_qml.h"
                  "cplusplus/header/pqc_scriptsexternaltools.h"
                  "cplusplus/header/pqc_scriptsexternaltools_qml.h"
)

if(WITH_VIDEO_MPV)
    set(previewqt_SRC ${previewqt_SRC} "cplusplus/header/pqc_mpvobject.h"
                                       "cplusplus/other/pqc_mpvobject.cpp")
endif()
