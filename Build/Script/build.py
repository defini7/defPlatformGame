from helpers import *
import platform


OS = platform.system()

DGE_PATH = '../../../defGameEngine'

if OS == 'Windows':
    GLFW_INCLUDE = 'C:/SDK/glfw/include'
    GLFW_LIB = 'C:/SDK/glfw/x64/lib-mingw-w64'
    STB_INCLUDE = 'C:/SDK/stb'
    SOL_INCLUDE = 'C:/SDK/sol2/include'
    LUA_PATH = 'C:/SDK/lua547/src'


def construct_engine_sources() -> list[str]:
    return [file_name for file_name in get_files(f'{DGE_PATH}/Sources/')]


def main():
    builder = Builder()

    create_dir_if_not_exists('../Target')

    c = Command('g++')

    c.add_flag('I"../../Include"')
    c.add_flag(f'I"{DGE_PATH}/Include"')
    c.add_flag('Wall')
    c.add_flag('pedantic')
    c.add_flag('std', 'c++20')
    c.add_flag('fms-extensions')
    c.add_flag('o')
    c.add_argument(f'../Target/Game')

    for source in construct_engine_sources():
        if source != 'PlatformEmscripten.cpp':
            c.add_argument(f'{DGE_PATH}/Sources/' + source)

    for target in get_files('../../Sources'):
        c.add_argument(f'../../Sources/{target}')

    match OS:
        case 'Windows':
            c.add_flag(f'I"{STB_INCLUDE}"')
            c.add_flag(f'I"{SOL_INCLUDE}"')
            c.add_flag(f'I"{LUA_PATH}"')
            c.add_flag(f'I"{GLFW_INCLUDE}"')
            c.add_flag(f'L"{GLFW_LIB}"')
            c.add_flag(f'L"{LUA_PATH}"')
            c.add_flag('lgdi32')
            c.add_flag('luser32')
            c.add_flag('lkernel32')
            c.add_flag('lopengl32')
            c.add_flag('lglfw3dll')
            c.add_flag('lglu32')
            c.add_flag('llua')
        case 'Linux':
            c.add_flag('lGL')
            c.add_flag('lGLU')
            c.add_flag('lglut')
            c.add_flag('lGLEW')
            c.add_flag('lglfw')
            c.add_flag('lX11')
            c.add_flag('lXxf86vm')
            c.add_flag('lXrandr')
            c.add_flag('lpthread')
            c.add_flag('lXi')
            c.add_flag('ldl')
            c.add_flag('lXinerama')
            c.add_flag('lXcursor')
            c.add_flag('lstb')
            c.add_flag('llua')
        case _:
            assert False, 'The builder is not implemeneted for your OS, please send an issue'

    builder.add_command(c)
    builder.execute(True, True, True)


if __name__ == '__main__':
    main()
