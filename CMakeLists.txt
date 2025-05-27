cmake_minimum_required(VERSION 3.30)
project(SO2_P2)

set(CMAKE_CXX_STANDARD 26)

add_executable(SO2_P2 main.cpp
        types.h
        patient.h
        safe_queue.cpp
        safe_queue.h
        resource_manager.cpp
        resource_manager.h
        configuration_watcher.cpp
        configuration_watcher.h
        registrar.cpp
        registrar.h
        arrival_dispatcher.cpp
        arrival_dispatcher.h
        triage.cpp
        triage.h
        department.cpp
        department.h
        doctor.cpp
        doctor.h
        deadlock_detector.cpp
        deadlock_detector.h
        metrics_collector.cpp
        metrics_collector.h
        monitor.cpp
        monitor.h
        hospital.cpp
        hospital.h
        config.cpp
        config.h
        patient_logger.cpp
        patient_logger.h)
