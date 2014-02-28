import sys
if sys.version_info[0] != 3:
    raise NameError("Python version 3 needed.")
sys.path.append("build")
import pylacore

core = pylacore.pylacore()
print(core.identify())

print(pylacore.uart())
