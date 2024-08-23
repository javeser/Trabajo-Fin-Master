import ctypes
import os
import numpy as np

# path to compiled library
lib_path = os.path.abspath('/l/javeser/farvertexing/build/libFarVertexing.so')
lib = ctypes.CDLL(lib_path)

# Definir tipos de retorno y argumentos para las funciones de la biblioteca compartida
lib.FarRKTrkTransport_new.restype = ctypes.POINTER(ctypes.c_char)
lib.FarRKTrkTransport_propagate_python.argtypes = [lib.FarRKTrkTransport_new.restype] + [ctypes.c_double for _ in range(9)] + [ctypes.POINTER(ctypes.c_double) for _ in range(4)]

lib.FarRKDOCA_new.restype = ctypes.POINTER(ctypes.c_char)
lib.FarRKDOCA_new.argtypes = [lib.FarRKTrkTransport_new.restype, ctypes.c_double ]
lib.eval.restype = ctypes.c_double
lib.eval.argtypes = [lib.FarRKDOCA_new.restype] + [ctypes.c_double for _ in range(12)]

# lib.FarRKIP_new.restype = ctypes.POINTER(ctypes.c_char)
# lib.FarRKIP_new.argtypes = [lib.FarRKTrkTransport_new.restype, ctypes.c_double ]
# lib.eval_IP.restype = ctypes.c_double
# lib.eval_IP.argtypes = [lib.FarRKIP_new.restype] + [ctypes.c_double for _ in range(9)]

          
class FarRKDOCA:
    def __init__(self, traj_transport, step):
        self.obj = lib.FarRKDOCA_new(traj_transport.obj, ctypes.c_double(step))

    def eval(self,x1,y1,z1,tx1,ty1,qoverp1, x2, y2, z2, tx2, ty2, qoverp2):
        return lib.eval(self.obj,ctypes.c_double(x1),ctypes.c_double(y1),ctypes.c_double(z1),ctypes.c_double(tx1),ctypes.c_double(ty1),ctypes.c_double(qoverp1),ctypes.c_double(x2),ctypes.c_double(y2),ctypes.c_double(z2),ctypes.c_double(tx2),ctypes.c_double(ty2),ctypes.c_double(qoverp2))

def propagate_to_vertex(vz, xi, yi, zi, pxi, pyi, pzi, qoverp, rk):
    xf, yf, txf, tyf = rk.propagate(vz/10., 1., xi/10., yi/10., zi/10., pxi/1000., pyi/1000., pzi/1000., qoverp*1000.)
    mom = np.array([txf, tyf, 1]) * ((np.abs(1 / (qoverp * 1000.))) / np.sqrt(txf**2 + tyf**2 + 1))
    return mom[0] * 1000., mom[1] * 1000., mom[2] * 1000., xf * 10., yf * 10.

# class FarRKIP:
#     def __init__(self, traj_transport, step):
#         self.obj = lib.FarRKIP_new(traj_transport.obj, ctypes.c_double(step))

#     def eval_IP(self,x, y, z, zRef, xRef, yRef, txRef, tyRef, qoverpRef):
#         return lib.eval_IP(self.obj,ctypes.c_double(x),ctypes.c_double(y),ctypes.c_double(z),ctypes.c_double(zRef), ctypes.c_double(xRef),ctypes.c_double(yRef), ctypes.c_double(txRef), ctypes.c_double(tyRef), ctypes.c_double(qoverpRef))

# Ejemplo de uso:
# rk = FarRKTrkTransport()
# doca = FarRKDOCA(rk, step=0.1, tolerance=0.01, maxiter=100, stepsearch=0.1, zmin=0.0, zmax=10.0)
# max_doca = doca.eval()
# print("Max DOCA:", max_doca)

# from rk_transport import FarRKTrkTransport

# my_rk = FarRKTrkTransport()

# my_doca = FarRKDOCA(my_rk,10)

# my_ip = FarRKIP(my_rk, 10)

# print(my_doca.eval(0,0,0,0,0,0,0,0,0,0,0,0)) # 12 inputs

# print(my_ip.eval_IP(0,0,0,0,0,0,0,0,0)) #9 inputs

