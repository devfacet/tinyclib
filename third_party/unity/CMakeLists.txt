include(FetchContent)

FetchContent_Declare(
  Unity
  GIT_REPOSITORY https://github.com/ThrowTheSwitch/Unity.git
  GIT_TAG cbcd08fa7de711053a3deec6339ee89cad5d2697 # v2.6.1
)
FetchContent_MakeAvailable(Unity)
