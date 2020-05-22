
aux_source_directory(. GSOAP_SRCS)

#target_sources(face_view PRIVATE ${GSOAP_SRCS})

add_library(gsoap ${GSOAP_SRCS})
