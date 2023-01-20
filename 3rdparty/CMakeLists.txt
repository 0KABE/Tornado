set(LIBRARY_NAME SRS)

add_library(${LIBRARY_NAME} "")
target_compile_features(${LIBRARY_NAME} PUBLIC cxx_std_20)
target_compile_definitions(${LIBRARY_NAME}
        PUBLIC
        # ASIO definition
        ASIO_USE_TS_EXECUTOR_AS_DEFAULT
        ASIO_NO_DEPRECATED
        # SRS ST definition
        LINUX
        DEBUG
        MD_HAVE_EPOLL
        MALLOC_STACK
        )

# Get SRS Directory
set(SRS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/srs/trunk)

# SRS Executable
add_executable(${LIBRARY_NAME}_EXE ${SRS_DIR}/src/main/srs_main_server.cpp)
target_link_libraries(${LIBRARY_NAME}_EXE PRIVATE ${LIBRARY_NAME})

###########################################################
# Start to configure SRS with jobs of number of CPUs.
include(ProcessorCount)
ProcessorCount(JOBS)

# We should always configure SRS for switching between branches.
IF (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    EXEC_PROGRAM("cd ${SRS_DIR} && ./configure --hds=off --srt=off --rtc=off --osx --jobs=${JOBS}")
ELSE ()
    EXEC_PROGRAM("cd ${SRS_DIR} && ./configure --hds=off --srt=off --rtc=off --jobs=${JOBS}")
ENDIF ()

set(DEPS_LIBS ${SRS_DIR}/objs/st/libst.a
        ${SRS_DIR}/objs/openssl/lib/libssl.a
        ${SRS_DIR}/objs/openssl/lib/libcrypto.a
        ${SRS_DIR}/objs/srtp2/lib/libsrtp2.a
        )
foreach (DEPS_LIB ${DEPS_LIBS})
    IF (NOT EXISTS ${DEPS_LIB})
        MESSAGE(FATAL_ERROR "${DEPS_LIB} not found")
    ELSE ()
        MESSAGE("${DEPS_LIB} is ok")
    ENDIF ()
endforeach ()

###########################################################
# Setup SRS project
TARGET_INCLUDE_DIRECTORIES(${LIBRARY_NAME}
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include

        ${SRS_DIR}/objs
        ${SRS_DIR}/objs/st
        ${SRS_DIR}/objs/hp
        ${SRS_DIR}/objs/openssl/include
        ${SRS_DIR}/objs/srtp2/include
        ${SRS_DIR}/objs/opus/include
        ${SRS_DIR}/objs/ffmpeg/include
        ${SRS_DIR}/src/core
        ${SRS_DIR}/src/kernel
        ${SRS_DIR}/src/protocol
        ${SRS_DIR}/src/app
        ${SRS_DIR}/src/service
        )

TARGET_COMPILE_OPTIONS(${LIBRARY_NAME} PRIVATE -g -O0)

TARGET_SOURCES(${LIBRARY_NAME}
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/entrypoint.cc
        # SRS source code
        ${SRS_DIR}/src/app/srs_app_async_call.cpp
        ${SRS_DIR}/src/app/srs_app_bandwidth.cpp
        ${SRS_DIR}/src/app/srs_app_caster_flv.cpp
        ${SRS_DIR}/src/app/srs_app_config.cpp
        ${SRS_DIR}/src/app/srs_app_conn.cpp
        ${SRS_DIR}/src/app/srs_app_coworkers.cpp
        ${SRS_DIR}/src/app/srs_app_dash.cpp
        ${SRS_DIR}/src/app/srs_app_dvr.cpp
        ${SRS_DIR}/src/app/srs_app_edge.cpp
        ${SRS_DIR}/src/app/srs_app_empty.cpp
        ${SRS_DIR}/src/app/srs_app_encoder.cpp
        ${SRS_DIR}/src/app/srs_app_ffmpeg.cpp
        ${SRS_DIR}/src/app/srs_app_forward.cpp
        ${SRS_DIR}/src/app/srs_app_fragment.cpp
        ${SRS_DIR}/src/app/srs_app_hds.cpp
        ${SRS_DIR}/src/app/srs_app_heartbeat.cpp
        ${SRS_DIR}/src/app/srs_app_hls.cpp
        ${SRS_DIR}/src/app/srs_app_hourglass.cpp
        ${SRS_DIR}/src/app/srs_app_http_api.cpp
        ${SRS_DIR}/src/app/srs_app_http_client.cpp
        ${SRS_DIR}/src/app/srs_app_http_conn.cpp
        ${SRS_DIR}/src/app/srs_app_http_hooks.cpp
        ${SRS_DIR}/src/app/srs_app_http_static.cpp
        ${SRS_DIR}/src/app/srs_app_http_stream.cpp
        ${SRS_DIR}/src/app/srs_app_hybrid.cpp
        ${SRS_DIR}/src/app/srs_app_ingest.cpp
        ${SRS_DIR}/src/app/srs_app_latest_version.cpp
        ${SRS_DIR}/src/app/srs_app_listener.cpp
        ${SRS_DIR}/src/app/srs_app_log.cpp
        ${SRS_DIR}/src/app/srs_app_mpegts_udp.cpp
        ${SRS_DIR}/src/app/srs_app_ng_exec.cpp
        ${SRS_DIR}/src/app/srs_app_pithy_print.cpp
        ${SRS_DIR}/src/app/srs_app_process.cpp
        ${SRS_DIR}/src/app/srs_app_recv_thread.cpp
        ${SRS_DIR}/src/app/srs_app_refer.cpp
        ${SRS_DIR}/src/app/srs_app_reload.cpp
        ${SRS_DIR}/src/app/srs_app_rtmp_conn.cpp
        ${SRS_DIR}/src/app/srs_app_security.cpp
        ${SRS_DIR}/src/app/srs_app_server.cpp
        ${SRS_DIR}/src/app/srs_app_source.cpp
        ${SRS_DIR}/src/app/srs_app_st.cpp
        ${SRS_DIR}/src/app/srs_app_statistic.cpp
        ${SRS_DIR}/src/app/srs_app_threads.cpp
        ${SRS_DIR}/src/app/srs_app_utility.cpp
        ${SRS_DIR}/src/app/srs_app_uuid.cpp
        ${SRS_DIR}/src/core/srs_core.cpp
        ${SRS_DIR}/src/core/srs_core_autofree.cpp
        ${SRS_DIR}/src/core/srs_core_performance.cpp
        ${SRS_DIR}/src/core/srs_core_time.cpp
        ${SRS_DIR}/src/core/srs_core_version4.cpp
        ${SRS_DIR}/src/kernel/srs_kernel_aac.cpp
        ${SRS_DIR}/src/kernel/srs_kernel_balance.cpp
        ${SRS_DIR}/src/kernel/srs_kernel_buffer.cpp
        ${SRS_DIR}/src/kernel/srs_kernel_codec.cpp
        ${SRS_DIR}/src/kernel/srs_kernel_consts.cpp
        ${SRS_DIR}/src/kernel/srs_kernel_error.cpp
        ${SRS_DIR}/src/kernel/srs_kernel_file.cpp
        ${SRS_DIR}/src/kernel/srs_kernel_flv.cpp
        ${SRS_DIR}/src/kernel/srs_kernel_io.cpp
        ${SRS_DIR}/src/kernel/srs_kernel_kbps.cpp
        ${SRS_DIR}/src/kernel/srs_kernel_log.cpp
        ${SRS_DIR}/src/kernel/srs_kernel_mp3.cpp
        ${SRS_DIR}/src/kernel/srs_kernel_mp4.cpp
        ${SRS_DIR}/src/kernel/srs_kernel_stream.cpp
        ${SRS_DIR}/src/kernel/srs_kernel_ts.cpp
        ${SRS_DIR}/src/kernel/srs_kernel_utility.cpp
        ${SRS_DIR}/src/protocol/srs_http_stack.cpp
        ${SRS_DIR}/src/protocol/srs_protocol_amf0.cpp
        ${SRS_DIR}/src/protocol/srs_protocol_format.cpp
        ${SRS_DIR}/src/protocol/srs_protocol_io.cpp
        ${SRS_DIR}/src/protocol/srs_protocol_json.cpp
        ${SRS_DIR}/src/protocol/srs_protocol_kbps.cpp
        ${SRS_DIR}/src/protocol/srs_protocol_stream.cpp
        ${SRS_DIR}/src/protocol/srs_protocol_utility.cpp
        ${SRS_DIR}/src/protocol/srs_raw_avc.cpp
        ${SRS_DIR}/src/protocol/srs_rtmp_handshake.cpp
        ${SRS_DIR}/src/protocol/srs_rtmp_msg_array.cpp
        ${SRS_DIR}/src/protocol/srs_rtmp_stack.cpp
        ${SRS_DIR}/src/protocol/srs_rtsp_stack.cpp
        ${SRS_DIR}/src/protocol/srs_service_conn.cpp
        ${SRS_DIR}/src/protocol/srs_service_http_client.cpp
        ${SRS_DIR}/src/protocol/srs_service_http_conn.cpp
        ${SRS_DIR}/src/protocol/srs_service_log.cpp
        ${SRS_DIR}/src/protocol/srs_service_rtmp_conn.cpp
        ${SRS_DIR}/src/protocol/srs_service_st.cpp
        ${SRS_DIR}/src/protocol/srs_service_utility.cpp
        )

TARGET_LINK_LIBRARIES(${LIBRARY_NAME}
        PUBLIC
        dl ${DEPS_LIBS} pthread
        )

MESSAGE(STATUS "@see https://github.com/ossrs/srs/wiki/v4_CN_IDE")
