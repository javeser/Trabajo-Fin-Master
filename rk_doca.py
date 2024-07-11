import ctypes
import os
import numpy as np

# path to compiled library
lib_path = os.path.abspath('/l/izaac/farvertexing/build/libFarVertexing.so')
lib = ctypes.CDLL(lib_path)

# Definir tipos de retorno y argumentos para las funciones de la biblioteca compartida
lib.FarRKTrkTransport_new.restype = ctypes.POINTER(ctypes.c_char)
lib.FarRKTrkTransport_propagate_python.argtypes = [lib.FarRKTrkTransport_new.restype] + [ctypes.c_double for _ in range(9)] + [ctypes.POINTER(ctypes.c_double) for _ in range(4)]

lib.FarRKDOCA_new.restype = ctypes.POINTER(ctypes.c_char)
lib.FarRKDOCA_eval.restype = ctypes.c_double
lib.FarRKDOCA_propagate_python.argtypes = [lib.FarRKDOCA_new.restype]+ [ctypes.c_double for _ in range(9)] + [ctypes.POINTER(ctypes.c_double) for _ in range(4)]

class FarRKTrkTransport:
    def __init__(self):
        self.obj = lib.FarRKTrkTransport_new(b'/l/izaac/farvertexing/db/MagneticFieldMapping20x20x100.root')
        
    def propagate(self, final_z, initial_step_size, initial_x, initial_y, initial_z, initial_px, initial_py, initial_pz, qoverp):
        final_z_c = ctypes.c_double(final_z)
        initial_step_size_c = ctypes.c_double(initial_step_size)
        initial_x_c = ctypes.c_double(initial_x)
        initial_y_c = ctypes.c_double(initial_y)
        initial_z_c = ctypes.c_double(initial_z)
        initial_px_c = ctypes.c_double(initial_px)
        initial_py_c = ctypes.c_double(initial_py)
        initial_pz_c = ctypes.c_double(initial_pz)
        qoverp_c = ctypes.c_double(qoverp)
        final_x = ctypes.c_double()
        final_y = ctypes.c_double()
        final_tx = ctypes.c_double()
        final_ty = ctypes.c_double()
        lib.FarRKTrkTransport_propagate_python(self.obj, final_z_c, initial_step_size_c, initial_x_c, initial_y_c, initial_z_c, initial_px_c, initial_py_c, initial_pz_c, qoverp_c, ctypes.byref(final_x), ctypes.byref(final_y), ctypes.byref(final_tx), ctypes.byref(final_ty))
        return final_x.value, final_y.value, final_tx.value, final_ty.value

class FarRKDOCA:
    def __init__(self, traj_transport, step):
        self.traj_transport = traj_transport
        self.obj = lib.FarRKDOCA_new(self.obj, traj_transport, ctypes.c_double(step))

    def eval(self):
        return lib.FarRKDOCA_eval(self.obj)

def propagate_to_vertex(vz, xi, yi, zi, pxi, pyi, pzi, qoverp, rk):
    xf, yf, txf, tyf = rk.propagate(vz/10., 1., xi/10., yi/10., zi/10., pxi/1000., pyi/1000., pzi/1000., qoverp*1000.)
    mom = np.array([txf, tyf, 1]) * ((np.abs(1 / (qoverp * 1000.))) / np.sqrt(txf**2 + tyf**2 + 1))
    return mom[0] * 1000., mom[1] * 1000., mom[2] * 1000., xf * 10., yf * 10.

# Ejemplo de uso:
# rk = FarRKTrkTransport()
# doca = FarRKDOCA(rk, step=0.1, tolerance=0.01, maxiter=100, stepsearch=0.1, zmin=0.0, zmax=10.0)
# max_doca = doca.eval()
# print("Max DOCA:", max_doca)
