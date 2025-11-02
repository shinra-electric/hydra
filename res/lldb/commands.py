import lldb


# Helpers
def execute_command(ci, command, result):
    ci.HandleCommand(command, result)
    if not result.Succeeded():
        raise RuntimeError(
            f'[LLDB Python] Error executing "{command}": ', result.GetError()
        )


def get_response(result):
    return result.GetOutput()[
        result.GetOutput().find("response:") + len("response:") :
    ].strip()


# Commands
def load_symbols(debugger, command, result, internal_dict):
    ci = debugger.GetCommandInterpreter()
    result = lldb.SBCommandReturnObject()

    execute_command(ci, "process plugin packet monitor getExecutables", result)
    executables = get_response(result).split(";")
    for executable in executables:
        path, addr = executable.rsplit(":", 1)
        path = path[1:-1]
        addr = int(addr, 0)

        execute_command(ci, f"target modules add {path}", result)
        execute_command(
            ci, f"target modules load --file {path} --slide {addr:#x}", result
        )
