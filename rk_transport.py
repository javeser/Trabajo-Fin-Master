import ctypes
import os
import numpy as np
# path to compiled library
lib_path = os.path.abspath('/l/javeser/farvertexing/build/libFarVertexing.so')
lib = ctypes.CDLL(lib_path)

lib.FarRKTrkTransport_new.restype = ctypes.POINTER(ctypes.c_char)
lib.FarRKTrkTransport_propagate_python.argtypes = [lib.FarRKTrkTransport_new.restype] + [ctypes.c_double for i in range (1,10)] + [ctypes.POINTER(ctypes.c_double) for i in range (1,4)]

class FarRKTrkTransport(object):    
    def __init__(self):
        # should probably update the path to match your own
        self.obj = lib.FarRKTrkTransport_new(b'/l/javeser/farvertexing/db/MagneticFieldMapping20x20x100.root')
        # self.obj = lib.FarRKTrkTransport_new(b'/l/izaac/farvertexing/db/MagneticFieldMapping100x100x500.root')

        
    def propagate(self,final_z, initial_step_size, initial_x, initial_y, initial_z, initial_px, initial_py, initial_pz, qoverp):
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
        lib.FarRKTrkTransport_propagate_python(self.obj,final_z_c,initial_step_size_c,initial_x_c,initial_y_c,initial_z_c,initial_px_c,initial_py_c,initial_pz_c,qoverp_c,ctypes.byref(final_x),ctypes.byref(final_y),ctypes.byref(final_tx),ctypes.byref(final_ty))
        # momentum can be calculated as
        # [px,py,pz] = [tx,ty,1] * p/|[tx,ty]|
        return final_x.value, final_y.value, final_tx.value, final_ty.value
    

### example:
# from rk_transport import FarRKTrkTransport
# my_rk_transport = FarRKTrkTransport()
# xf, yf, txf, tyf = my_rk_transport.propagate(final_z,intial_stepsize,xi,yi,zi,pxi,pyi,pzi,qoverp)


def propagate_to_vertex(vz, xi, yi, zi, pxi, pyi, pzi, qoverp,rk):
    xf,yf,txf,tyf = rk.propagate(vz/10., 1., xi/10., yi/10., zi/10., pxi/1000., pyi/1000., pzi/1000., qoverp*1000.)
    mom = np.array([txf,tyf,1])*((np.abs(1/(qoverp*1000.)))/np.sqrt(txf**2. + tyf**2. + 1.))
    return mom[0]*1000., mom[1]*1000., mom[2]*1000., xf*10., yf*10.

### example:
# rk = FarRKTrkTransport()
# vertex_values = pd.DataFrame(columns=['p_PX_V','p_PY_V','p_PZ_V','p_X_V','p_Y_V'])
#
# vertex_values[['p_PX_V','p_PY_V','p_PZ_V','p_X_V','p_Y_V']] = mc_data.apply(lambda row: propagate_to_vertex(row.L_END_VZ,row.p_TRACK_POS_CLOSESTTOBEAM_X,row.p_TRACK_POS_CLOSESTTOBEAM_Y,row.p_TRACK_POS_CLOSESTTOBEAM_Z,row.p_PX,row.p_PY,row.p_PZ,row.p_QOVERP,rk),axis='columns',result_type='expand')
#
# mc_data = pd.concat([mc_data,vertex_values],axis='columns')
