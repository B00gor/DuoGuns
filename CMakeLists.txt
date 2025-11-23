cmake_minimum_required(VERSION 3.16)

project(DuoGuns LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
find_package(ftxui REQUIRED)

add_executable(DuoGuns main.cpp
  config.h
  chatmessage.h chatmessage.cpp
  gaster.h gaster.cpp
  shooter.h shooter.cpp
  defender.h defender.cpp
  team.h team.cpp
  defensesystem.h defensesystem.cpp
  gastergenerator.h gastergenerator.cpp
  chatmanager.h chatmanager.cpp
  gameengine.h gameengine.cpp
  teamselectionscreen.h teamselectionscreen.cpp
  mainmenu.h mainmenu.cpp
  application.h application.cpp
  battlecontroller.h battlecontroller.cpp
  battleview.h battleview.cpp)

target_link_libraries(DuoGuns
  PRIVATE ftxui::screen
  PRIVATE ftxui::dom
  PRIVATE ftxui::component
)
include(GNUInstallDirs)
install(TARGETS DuoGuns
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
