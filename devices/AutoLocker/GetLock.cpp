/*************************************************************************
 *
 * Name:   GetLock.h
 *
 * C++ Windows header for scanning and locking the Vortex
 *
 * Susannah Dickerson 7/8/2008
 * Dave Johnson
 * Kasevich Group - Stanford University
 *
 *
 **************************************************************************/

#include "GetLock.h"


// Constructor
GETLOCK::GETLOCK(Vortex6000 &vortex6000)
{
	lockVoltage = 0;
	vortexVoltage = vortex6000.get_piezo_voltage();
	start_voltage = 0; // start point in GHz
	voltage_incr = .001; //increment frequency in GHz
	end_voltage = .260; // endpoint in GHz   
	usb_input_channel = 3;
	usb_output_channel = 0;

	GHzToV = .046/.700; // .700 GHz == .046 V
}

// Destructor
GETLOCK::~GETLOCK()
{
}



/*************************************************************************
 * setVortexVoltage-- Public
 * Description-- accesses the Vortex and sets the voltage
 * Input-- voltage, the voltage to send to the Vortex
 *         usb1408fs, address of the usb object.
 * Output-- none
 *************************************************************************/
void GETLOCK::setVortexVoltage (double voltage, Vortex6000 &vortex6000)
{
	lockVoltage = voltage;

	vortex6000.set_piezo_voltage(vortexVoltage + .1 * ceil(lockVoltage/.019));
}



/*************************************************************************
 * lock-- Public
 * Description-- scan the Vortex voltage, find the Rb 85 cooling peak
 *         turn on the sidebands, and find the blue-shifted Rb 85 cooling
 *         peaks. Set the vortex voltage to this value.
 * Input-- offsetGHz_p, a pointer to the position where the sideband
 *             frequency offset should be stored.
 *         matlabplotter, address of the matlab object.
 *         usb1408fs, address of the usb object.
 *         agilent8648a, address of the frequency generator object.
 * Output-- user instructions
 * Return-- bool. False if finding the lock voltage was successful.
 *************************************************************************/
bool GETLOCK::lock (double* offsetGHz_p, MATLABPLOTTER &matlabplotter, 
					USB1408FS &usb1408fs, AGILENT8648A &agilent8648a,
					Vortex6000 &vortex6000)
{
	// RangeV should be roughly twice the expected width of the Rb 85 cooling 
	//     transitions. When decreasing rangeV, ensure that windowV in 
	//     findSidebandPeaks is still reasonable.
	double rangeV = .6 * GHzToV;

	double coolingPeakV;
	double sidebandPeak;
	int startSideband;
	int endSideband;
	bool noScan;
	bool change_vals = true;

	std::vector <double> voltage_vector;
	std::vector <double> DAQ_vector;
	std::vector <double> voltageSB_vector;
	std::vector <double> DAQSB_vector;

	//manual data
//	int i;
//	double voltageData[]={0.001, 0.002, 0.003, 0.004, 0.005, 0.006, 0.007, 0.008, 0.009, 0.01, 0.011, 0.012, 0.013, 0.014, 0.015, 0.016, 0.017, 0.018, 0.019, 0.02, .021, 0.022, 0.023, 0.024, 0.025, 0.026, 0.027, 0.028, 0.029, 0.03, 0.031, 0.032, 0.033, 0.034, 0.035, 0.036, 0.037, 0.038, 0.039, 0.04, 0.041, 0.042, 0.043, 0.044, 0.045, 0.046, 0.047, 0.048, 0.049, 0.05, 0.051, 0.052, 0.053, 0.054, 0.055, 0.056, 0.057, 0.058, 0.059, 0.06, 0.061, 0.062, 0.063, 0.064, 0.065, 0.066, 0.067, 0.068, 0.069, 0.07, 0.071, 0.072, 0.073, 0.074, 0.075, 0.076, 0.077, 0.078, 0.079, 0.08, 0.081, 0.082, 0.083, 0.084, 0.085, 0.086, 0.087, 0.088, 0.089, 0.09, 0.091, 0.092, 0.093, 0.094, 0.095, 0.096, 0.097, 0.098, 0.099, 0.1, 0.101, 0.102, 0.103, 0.104, 0.105, 0.106, 0.107, 0.108, 0.109, 0.11, 0.111, 0.112, 0.113, 0.114, 0.115, 0.116, 0.117, 0.118, 0.119, 0.12, 0.121, 0.122, 0.123, 0.124, 0.125, 0.126, 0.127, 0.128, 0.129, 0.13, 0.131, 0.132, 0.133, 0.134, 0.135, 0.136, 0.137, 0.138, 0.139, 0.14, 0.141, 0.142, 0.143, 0.144, 0.145, 0.146, 0.147, 0.148, 0.149, 0.15, 0.151, 0.152, 0.153, 0.154, 0.155, 0.156, 0.157, 0.158, 0.159, 0.16, 0.161, 0.162, 0.163, 0.164, 0.165, 0.166, 0.167, 0.168, 0.169, 0.17, 0.171, 0.172, 0.173, 0.174, 0.175, 0.176, 0.177, 0.178, 0.179, 0.18, 0.181, 0.182, 0.183, 0.184, 0.185, 0.186, 0.187, 0.188, 0.189, 0.19, 0.191, 0.192, 0.193, 0.194, 0.195, 0.196, 0.197, 0.198, 0.199, 0.2, 0.201, 0.202, 0.203, 0.204, 0.205, 0.206, 0.207, 0.208, 0.209, 0.21, 0.211, 0.212, 0.213, 0.214, 0.215, 0.216, 0.217, 0.218, 0.219, 0.22, 0.221, 0.222, 0.223, 0.224, 0.225, 0.226, 0.227, 0.228, 0.229, 0.23, 0.231, 0.232, 0.233, 0.234, 0.235, 0.236, 0.237, 0.238, 0.239, 0.24, 0.241, 0.242, 0.243, 0.244, 0.245, 0.246, 0.247, 0.248, 0.249, 0.25, 0.251, 0.252, 0.253, 0.254, 0.255, 0.256, 0.257, 0.258, 0.259, 0.26, 0.261, 0.262, 0.263, 0.264, 0.265, 0.266, 0.267, 0.268, 0.269, 0.27, 0.271, 0.272, 0.273, 0.274, 0.275, 0.276, 0.277, 0.278, 0.279, 0.28, 0.281, 0.282, 0.283, 0.284, 0.285, 0.286, 0.287, 0.288, 0.289, 0.29, 0.291, 0.292, 0.293, 0.294, 0.295, 0.296, 0.297, 0.298, 0.299, 0.3, 0.301, 0.302, 0.303, 0.304, 0.305, 0.306, 0.307, 0.308, 0.309, 0.31, 0.311, 0.312, 0.313, 0.314, 0.315, 0.316, 0.317, 0.318, 0.319, 0.32, 0.321, 0.322, 0.323, 0.324, 0.325, 0.326, 0.327, 0.328, 0.329, 0.33, 0.331, 0.332, 0.333, 0.334, 0.335, 0.336, 0.337, 0.338, 0.339, 0.34, 0.341, 0.342, 0.343, 0.344, 0.345, 0.346, 0.347, 0.348, 0.349, 0.35, 0.351, 0.352, 0.353, 0.354, 0.355, 0.356, 0.357, 0.358, 0.359, 0.36, 0.361, 0.362, 0.363, 0.364, 0.365, 0.366, 0.367, 0.368, 0.369, 0.37, 0.371, 0.372, 0.373, 0.374, 0.375, 0.376, 0.377, 0.378, 0.379, 0.38, 0.381, 0.382, 0.383, 0.384, 0.385, 0.386, 0.387, 0.388, 0.389, 0.39, 0.391, 0.392, 0.393, 0.394, 0.395, 0.396, 0.397, 0.398, 0.399, 0.4, 0.401, 0.402, 0.403, 0.404, 0.405, 0.406, 0.407, 0.408, 0.409, 0.41, 0.411, 0.412, 0.413, 0.414, 0.415, 0.416, 0.417, 0.418, 0.419, 0.42, 0.421, 0.422, 0.423, 0.424, 0.425, 0.426, 0.427, 0.428, 0.429, 0.43, 0.431, 0.432, 0.433, 0.434, 0.435, 0.436, 0.437, 0.438, 0.439, 0.44, 0.441, 0.442, 0.443, 0.444, 0.445, 0.446, 0.447, 0.448, 0.449, 0.45, 0.451, 0.452, 0.453, 0.454, 0.455, 0.456, 0.457, 0.458, 0.459, 0.46, 0.461, 0.462, 0.463, 0.464, 0.465, 0.466, 0.467, 0.468, 0.469, 0.47, 0.471, 0.472, 0.473, 0.474, 0.475, 0.476, 0.477, 0.478, 0.479, 0.48, 0.481, 0.482, 0.483, 0.484, 0.485, 0.486, 0.487, 0.488, 0.489, 0.49, 0.491, 0.492, 0.493, 0.494, 0.495, 0.496, 0.497, 0.498, 0.499, 0.5, 0.501, 0.502, 0.503, 0.504, 0.505, 0.506, 0.507, 0.508, 0.509, 0.51, 0.511, 0.512, 0.513, 0.514, 0.515, 0.516, 0.517, 0.518, 0.519, 0.52, 0.521, 0.522, 0.523, 0.524, 0.525, 0.526, 0.527, 0.528, 0.529, 0.53, 0.531, 0.532, 0.533, 0.534, 0.535, 0.536, 0.537, 0.538, 0.539, 0.54, 0.541, 0.542, 0.543, 0.544, 0.545, 0.546, 0.547, 0.548, 0.549, 0.55, 0.551, 0.552, 0.553, 0.554, 0.555, 0.556, 0.557, 0.558, 0.559, 0.56, 0.561, 0.562, 0.563, 0.564, 0.565, 0.566, 0.567, 0.568, 0.569, 0.57, 0.571, 0.572, 0.573, 0.574, 0.575, 0.576, 0.577, 0.578, 0.579, 0.58, 0.581, 0.582, 0.583, 0.584, 0.585, 0.586, 0.587, 0.588, 0.589, 0.59, 0.591, 0.592, 0.593, 0.594, 0.595, 0.596, 0.597, 0.598, 0.599};
//	double DAQData[]={1.9531, 2.0642, 2.0764, 2.0667, 2.0715, 2.0715, 2.0813, 2.0715, 2.0886, 2.0813, 2.0691, 2.0764, 2.0557, 2.0667, 2.041, 2.0715, 2.0435, 2.0532, 2.0337, 2.0483, 2.0264, 2.0081, 2.0361, 2.0117, 2.0056, 1.9861, 1.9958, 1.9556, 2.019, 1.9983, 2.0056, 1.9934, 1.9788, 1.9666, 2.0007, 1.969, 1.9763, 1.9739, 2.0056, 2.0032, 2.0032, 1.9885, 1.9507, 2.0264, 1.958, 1.9556, 1.9666, 1.8738, 1.8005, 1.7505, 1.8005, 1.748, 1.7505, 1.7139, 1.7615, 1.7261, 1.7114, 1.7029, 1.7456, 1.7114, 1.7261, 1.7163, 1.7163, 1.7981, 1.7859, 1.7432, 1.8286, 1.8408, 1.8713, 1.8054, 1.8884, 1.7957, 1.9263, 1.9055, 1.9763, 1.9434, 1.9531, 1.9958, 1.9934, 1.9666, 2.0056, 2.0361, 2.0361, 2.0459, 2.0361, 2.0557, 2.0435, 2.0557, 2.0557, 2.0532, 2.0581, 2.0239, 2.0605, 2.0557, 2.0288, 2.0081, 1.9531, 1.9617, 1.936, 1.9556, 1.9434, 1.9116, 1.8457, 1.8811, 1.8457, 1.759, 1.7786, 1.6833, 1.7139, 1.6809, 1.6711, 1.6406, 1.6736, 1.604, 1.604, 1.6931, 1.604, 1.7358, 1.6138, 1.7688, 1.886, 1.7261, 1.8787, 1.8481, 1.7432, 1.4734, 1.5564, 1.5161, 1.4587, 1.4685, 1.4539, 1.4539, 1.4404, 1.4539, 1.4685, 1.4734, 1.5515, 1.5564, 1.5356, 1.5955, 1.5759, 1.7065, 1.6357, 1.7761, 1.6504, 1.7639, 1.8689, 1.7566, 1.8811, 1.8481, 1.9287, 1.8616, 1.886, 1.9031, 1.9763, 1.9836, 2.0508, 2.0239, 2.0581, 2.0288, 2.0959, 2.0557, 2.0886, 2.1082, 2.1057, 2.0935, 2.1008, 2.0935, 2.1033, 2.1057, 2.1033, 2.1082, 2.1033, 2.1057, 2.1167, 2.1143, 2.1191, 2.1216, 2.1216, 2.1216, 2.1265, 2.1265, 2.1265, 2.1265, 2.124, 2.1265, 2.124, 2.1265, 2.1216, 2.1265, 2.124, 2.1265, 2.124, 2.1265, 2.1265, 2.1265, 2.1265, 2.1313, 2.1216, 2.1216, 2.1216, 2.1216, 2.1216, 2.1167, 2.1216, 2.1216, 2.1167, 2.1143, 2.1082, 2.1057, 2.1106, 2.1167, 2.1167, 2.1167, 2.1167, 2.1143, 2.1143, 2.1167, 2.1106, 2.1143, 2.1191, 2.1167, 2.1167, 2.1106, 2.1167, 2.1143, 2.1167, 2.1167, 2.1216, 2.1216, 2.124, 2.1265, 2.1265, 2.1265, 2.1216, 2.1191, 2.1216, 2.124, 2.124, 2.1265, 2.1265, 2.124, 2.1265, 2.1265, 2.1265, 2.1265, 2.124, 2.1289, 2.1265, 2.1289, 2.1265, 2.1265, 2.124, 2.1265, 2.124, 2.124, 2.1143, 2.1167, 2.1167, 2.1143, 2.1143, 2.1057, 2.1033, 2.0984, 2.1033, 2.1008, 2.1033, 2.0935, 2.0935, 2.0959, 2.1008, 2.1008, 2.0837, 2.0605, 2.0581, 2.0557, 2.0264, 2.041, 2.0215, 2.0264, 2.0215, 2.0215, 1.991, 2.0056, 1.991, 1.9714, 1.9958, 1.9812, 1.9617, 1.9788, 1.9739, 1.9556, 1.9812, 1.9861, 1.9958, 2.0337, 2.0264, 1.9958, 2.0056, 2.0239, 2.0337, 1.9885, 1.9763, 1.9812, 1.9763, 1.9788, 1.9763, 1.9885, 1.9836, 1.991, 2.0056, 2.0239, 2.0056, 2.0117, 2.0313, 2.0508, 2.0483, 2.0667, 2.0581, 2.074, 2.0764, 2.0789, 2.1033, 2.0862, 2.0862, 2.0959, 2.1008, 2.1082, 2.1167, 2.1167, 2.1143, 2.1167, 2.1216, 2.1167, 2.1167, 2.1167, 2.1167, 2.1143, 2.1167, 2.1167, 2.1167, 2.1106, 2.1143, 2.1143, 2.1082, 2.1143, 2.1082, 2.1191, 2.1167, 2.1191, 2.1216, 2.1216, 2.1167, 2.1167, 2.1216, 2.1143, 2.1143, 2.1167, 2.124, 2.124, 2.1289, 2.1289, 2.1265, 2.1265, 2.1265, 2.1216, 2.1216, 2.124, 2.124, 2.1216, 2.124, 2.1289, 2.1289, 2.1289, 2.1289, 2.1265, 2.1289, 2.1191, 2.1216, 2.124, 2.1265, 2.1289, 2.1338, 2.124, 2.1265, 2.1265, 2.1216, 2.1265, 2.1216, 2.1143, 2.1082, 2.1143, 2.124, 2.1216, 2.1216, 2.1167, 2.1265, 2.124, 2.1265, 2.124, 2.1265, 2.1265, 2.1191, 2.1216, 2.1191, 2.1167, 2.1143, 2.1167, 2.1057, 2.1191, 2.1057, 2.1167, 2.1106, 2.1008, 2.1057, 2.0984, 2.1057, 2.0959, 2.1082, 2.1033, 2.0886, 2.0862, 2.0911, 2.0837, 2.0789, 2.0789, 2.0813, 2.0862, 2.0715, 2.0764, 2.0764, 2.0715, 2.074, 2.0764, 2.0789, 2.0789, 2.0862, 2.0935, 2.0862, 2.0837, 2.0911, 2.0862, 2.0837, 2.0935, 2.0959, 2.0862, 2.0837, 2.0813, 2.0862, 2.0789, 2.0911, 2.0935, 2.0935, 2.0959, 2.1008, 2.1033, 2.1082, 2.1082, 2.1008, 2.1033, 2.1106, 2.1167, 2.1167, 2.1167, 2.1191, 2.1216, 2.124, 2.1265, 2.1216, 2.1216, 2.124, 2.1313, 2.1265, 2.1313, 2.1289, 2.1338, 2.1265, 2.1265, 2.1362, 2.1362, 2.1338, 2.1362, 2.1387, 2.1362, 2.1362, 2.1411, 2.1387, 2.1362, 2.1362, 2.1362, 2.1411, 2.1533, 2.146, 2.1411, 2.146, 2.1411, 2.146, 2.1387, 2.1362, 2.1338, 2.1338, 2.1362, 2.1362, 2.1338, 2.1338, 2.1362, 2.1265, 2.1265, 2.1338, 2.1338, 2.1411, 2.1362, 2.1289, 2.1265, 2.1362, 2.1362, 2.1411, 2.1436, 2.146, 2.1436, 2.146, 2.146, 2.146, 2.1533, 2.1484, 2.146, 2.1436, 2.1484, 2.1484, 2.1533, 2.1484, 2.146, 2.1484, 2.1509, 2.1484, 2.1509, 2.1533, 2.1533, 2.1558, 2.1509, 2.1509, 2.1509, 2.1509, 2.1533, 2.146, 2.1509, 2.146, 2.1509, 2.1582, 2.1558, 2.1533, 2.1558, 2.1558, 2.1606, 2.1558, 2.1533, 2.1509, 2.1533, 2.1582, 2.1509, 2.1484, 2.1436, 2.1411, 2.1411, 2.146, 2.1533, 2.1533, 2.1509, 2.1484, 2.1533, 2.1558, 2.1606, 2.1606, 2.1558, 2.1582, 2.1558, 2.1558, 2.1558, 2.1558, 2.1558, 2.1558, 2.1582, 2.1582, 2.1582, 2.1643, 2.1582, 2.1484, 2.1533, 2.1582, 2.1667, 2.1606, 2.1692, 2.1692, 2.1667, 2.1667, 2.1667, 2.1741, 2.1716, 2.1692};
//	double DAQData[]={1.958, 1.9763, 2.0056, 1.9763, 1.9714, 1.9556, 1.9836, 1.9531, 2.0166, 1.9812, 1.9861, 2.019, 2.0081, 1.991, 1.991, 2.0166, 2.0239, 1.9958, 1.9885, 2.0215, 1.9983, 1.9861, 1.9739, 2.019, 2.0007, 1.9531, 1.9617, 1.969, 1.9556, 1.9556, 1.9434, 1.9336, 1.9287, 1.9336, 1.9458, 1.9666, 1.9788, 1.9482, 1.9238, 1.9214, 1.936, 1.9641, 1.936, 1.9714, 1.969, 1.9482, 1.8433, 1.8262, 1.8237, 1.8311, 1.886, 1.8335, 1.8555, 1.8188, 1.781, 1.7639, 1.8005, 1.7786, 1.7712, 1.7712, 1.7664, 1.7834, 1.8262, 1.7664, 1.7639, 1.7883, 1.7957, 1.7859, 1.7981, 1.8164, 1.8054, 1.7981, 1.7737, 1.8555, 1.7688, 1.7566, 1.8091, 1.8481, 1.8359, 1.7786, 1.7334, 1.731, 1.7456, 1.7505, 1.7639, 1.7456, 1.731, 1.7236, 1.7358, 1.7615, 1.748, 1.7761, 1.7432, 1.7737, 1.853, 1.7859, 1.7834, 1.7432, 1.8433, 1.8237, 1.8555, 1.7358, 1.8005, 1.7432, 1.7358, 1.731, 1.7334, 1.6858, 1.698, 1.6504, 1.676, 1.6687, 1.6858, 1.6687, 1.6638, 1.6565, 1.6833, 1.6809, 1.6736, 1.6687, 1.8762, 1.853, 1.6956, 1.8762, 1.7639, 1.7407, 1.7236, 1.6479, 1.6504, 1.6931, 1.6431, 1.6479, 1.6541, 1.6858, 1.6565, 1.6638, 1.6858, 1.6858, 1.7688, 1.7236, 1.9165, 1.8115, 1.7786, 1.9165, 1.8689, 1.8506, 1.8591, 1.8665, 1.8091, 1.8506, 1.7957, 1.7981, 1.7981, 1.8115, 1.8054, 1.8054, 1.814, 1.803, 1.803, 1.8359, 1.8237, 1.9287, 1.8054, 1.9116, 1.9116, 1.9287, 1.9214, 1.936, 1.8481, 1.8286, 1.8433, 1.8311, 1.7957, 1.781, 1.7957, 1.8054, 1.8433, 1.8359, 1.8115, 1.8188, 1.8933, 1.8286, 1.8433, 1.8665, 1.9031, 1.8811, 1.9434, 1.9617, 1.9556, 1.9336, 1.9409, 1.9336, 1.9458, 1.936, 1.9385, 1.9531, 2.0056, 2.0032, 1.9861, 2.0056, 2.0215, 2.019, 2.0215, 2.0215, 2.0264, 2.0239, 2.0264, 2.0239, 2.0215, 2.019, 2.0215, 2.0264, 2.0215, 2.0337, 2.0264, 2.0337, 2.0288, 2.0361, 2.0361, 2.0361, 2.0361, 2.0337, 2.0386, 2.0361, 2.0361, 2.041, 2.0435, 2.041, 2.041, 2.0337, 2.0361, 2.0313, 2.0459, 2.0361, 2.0483, 2.0435, 2.041, 2.0508, 2.0483, 2.0435, 2.0313, 2.0239, 2.0215, 2.019, 2.0264, 2.0288, 2.019, 2.0117, 1.9983, 2.0142, 1.9983, 2.0166, 2.0032, 2.0081, 2.0386, 2.0288, 2.0166, 2.0264, 2.0386, 2.0361, 2.0117, 2.0337, 2.0117, 2.0264, 2.0081, 2.0117, 2.0081, 2.0117, 2.0142, 2.0166, 2.0215, 2.0264, 2.0215, 2.0215, 2.041, 2.0361, 2.0459, 2.0288, 2.0435, 2.0081, 2.0337, 2.0081, 2.0386, 1.9983, 2.0056, 1.9861, 1.9861, 1.9763, 1.9885, 1.9812, 1.991, 1.9861, 1.9958, 1.9934, 1.9934, 1.9983, 2.0215, 1.9983, 2.0264, 2.0032, 1.9861, 1.9861, 1.9934, 1.9885, 1.9861, 1.9836, 1.991, 1.991, 1.9836, 2.0056, 2.0117, 1.991, 2.0081, 2.0142, 2.0264, 2.0215, 2.0483, 2.0386, 2.0557, 2.0557, 2.0557, 2.0361, 2.0386, 2.0361, 2.0459, 2.0288, 2.0264, 2.0142, 2.0239, 2.0264, 2.019, 2.0142, 2.019, 2.0166, 2.0166, 2.041, 2.0435, 2.0313, 2.019, 2.0459, 2.0337, 2.0337, 2.0142, 2.0166, 2.0142, 2.0142, 2.0142, 2.0142, 2.0117, 2.0166, 2.0142, 2.0288, 2.0361, 2.0239, 2.0264, 2.0459, 2.0532, 2.0605, 2.0557, 2.0483, 2.0532, 2.0642, 2.0642, 2.0667, 2.0764, 2.074, 2.0764, 2.0715, 2.0715, 2.0715, 2.0764, 2.0764, 2.0764, 2.0789, 2.0813, 2.0813, 2.0837, 2.0837, 2.0837, 2.0813, 2.0813, 2.0764, 2.074, 2.074, 2.0764, 2.0764, 2.074, 2.0667, 2.0715, 2.0715, 2.0764, 2.0691, 2.0715, 2.0715, 2.074, 2.0691, 2.0667, 2.0667, 2.0837, 2.0764, 2.0764, 2.0813, 2.0813, 2.0667, 2.0789, 2.0813, 2.0862, 2.0813, 2.0813, 2.0789, 2.0789, 2.0764, 2.0764, 2.0813, 2.0862, 2.0862, 2.0862, 2.0813, 2.0813, 2.0764, 2.0764, 2.0813, 2.0764, 2.0764, 2.0813, 2.0813, 2.0789, 2.0715, 2.0764, 2.0764, 2.0837, 2.0813, 2.0862, 2.0862, 2.0886, 2.0789, 2.0862, 2.0789, 2.0789, 2.0764, 2.0789, 2.0886, 2.0837, 2.0862, 2.0837, 2.0813, 2.0813, 2.0862, 2.0886, 2.0911, 2.0935, 2.0959, 2.0935, 2.1008, 2.0911, 2.0911, 2.0959, 2.1008, 2.1033, 2.1033, 2.0959, 2.0935, 2.0911, 2.0959, 2.0886, 2.0886, 2.0789, 2.0789, 2.0764, 2.0789, 2.0813, 2.0862, 2.0862, 2.0862, 2.0911, 2.0886, 2.0862, 2.0984, 2.0935, 2.0984, 2.0935, 2.0862, 2.0935, 2.0984, 2.0959, 2.0959, 2.0984, 2.1008, 2.0959, 2.1008, 2.1033, 2.1033, 2.1008, 2.1008, 2.0959, 2.1008, 2.1057, 2.1082, 2.1191, 2.1265, 2.1289, 2.1313, 2.1265, 2.1265, 2.124, 2.1265, 2.1265, 2.1265, 2.1265, 2.1265, 2.1265, 2.1216, 2.1216, 2.1265, 2.1265, 2.1265, 2.1265, 2.1265, 2.1265, 2.1338, 2.1338, 2.1338, 2.1289, 2.124, 2.1289, 2.1289, 2.1289, 2.1313, 2.1265, 2.1338, 2.1338, 2.1362, 2.1362, 2.1338, 2.1411, 2.1411, 2.1436, 2.1387, 2.1436, 2.1411, 2.1436, 2.1436, 2.1362, 2.1411, 2.1411, 2.1411, 2.1411, 2.1387, 2.1436, 2.1436, 2.1436, 2.1338, 2.1338, 2.1338, 2.1387, 2.1362, 2.1313, 2.1338, 2.1362, 2.1338, 2.1265, 2.1313, 2.1289, 2.1362, 2.1362, 2.1362, 2.1362, 2.1411, 2.1411, 2.1436, 2.146, 2.146, 2.146, 2.146, 2.1509, 2.1509, 2.1533, 2.1484, 2.1533, 2.146, 2.146, 2.1509, 2.146, 2.1484, 2.1509, 2.146, 2.1509, 2.146, 2.146, 2.1411, 2.1436, 2.146, 2.146, 2.146, 2.146, 2.1509, 2.1509, 2.1533, 2.1533};

	// Get user input about the RbScanner parameters
	getParameters();

	noScan = scan(voltage_vector, DAQ_vector, usb1408fs);
	if (noScan) {return (1);}

	plot(voltage_vector, DAQ_vector, matlabplotter);

	// Find the Rb 85 cooling peak. 
	coolingPeakV = findCoolingPeak(voltage_vector, DAQ_vector, 
		0, voltage_vector.size());
	//std::cerr <<std::endl << coolingPeakV <<std::endl;

	std::cout << "Offset Lock frequency: " << *offsetGHz_p << " GHz" <<
		std::endl;
	std::cout << "Do you wish to change the offset frequency? (1/0) ";
    std::cin >> change_vals; 

    if(change_vals) {

        // user defined sideband frequency
		std::cout << "Enter desired offset lock frequency in GHz. " <<
			"Default value is: " << *offsetGHz_p << " GHz" << std::endl <<
			"frequency = ";
		std::cin >> *offsetGHz_p;
    }

	// Turn on the sidebands
	agilent8648a.output_on();
	agilent8648a.set_frequency(*offsetGHz_p);

	// Record the range where we expect to see the 85 cooling sideband peaks
	startSideband = position(voltage_vector, 
		coolingPeakV + *offsetGHz_p * GHzToV - rangeV / 2);
	endSideband = position(voltage_vector, 
		coolingPeakV + *offsetGHz_p * GHzToV + rangeV / 2);

	voltage_vector.clear();
	DAQ_vector.clear();
	
	// Scan the vortex and plot the data. With sidebands
	noScan = scan(voltage_vector, DAQ_vector, usb1408fs);
	if (noScan) {return (1);}

	// Find the peak of the sideband Rb 85 cooling transition
	sidebandPeak = findSidebandPeak(voltage_vector, DAQ_vector,
		startSideband, endSideband);

	voltageSB_vector.push_back(voltage_vector.at(startSideband));
	DAQSB_vector.push_back(DAQ_vector.at(startSideband));
	voltageSB_vector.push_back(sidebandPeak);
	DAQSB_vector.push_back(DAQ_vector.at(position(voltage_vector,
		sidebandPeak)));
	voltageSB_vector.push_back(voltage_vector.at(endSideband));
	DAQSB_vector.push_back(DAQ_vector.at(endSideband));

	plot(voltage_vector, DAQ_vector, voltageSB_vector, DAQSB_vector,
		matlabplotter);

	// Set the Vortex voltage to the Rb 85 cooling peak.
	usb1408fs.set_output_voltage(usb_output_channel, 0);
	setVortexVoltage(sidebandPeak, vortex6000);

	return(0);
}



/*************************************************************************
 * getParameters-- Private
 * Description-- Prints default lock values and accepts user input to
 *         change them.
 * Input-- user input
 * Output-- user instructions
 *************************************************************************/
void GETLOCK::getParameters ()
{
    bool change_vals = true; // have user defined values

	std::cout << "default values are as follows:" << std::endl;
	std::cout << "USB input channel: " << usb_input_channel << std::endl;
	std::cout << "USB output channel: " << usb_output_channel << std::endl;
    std::cout << "Start Voltage: " << start_voltage << " GHz" << std::endl;
    std::cout << "End Voltage: " << end_voltage << " GHz" << std::endl;
    std::cout << "Voltage Increment: " << voltage_incr << " GHz" << std::endl;
    std::cout << std::endl << "Do you want to change (1/0)? ";
    std::cin >> change_vals; 

    if(change_vals) {

        // user defined start frequency
        std::cout << "Enter desired start voltage in volts. " <<
			"Default value is: " << start_voltage << " V" << std::endl <<
			"start voltage = ";
        std::cin >> start_voltage;
   
        std::cout << "Enter desired end voltage in volts. " <<
			"Default value is: " << end_voltage << " V" << std::endl <<
			"end voltage = ";
        std::cin >> end_voltage;
   
        std::cout << "Enter desired voltage increment in volts. " <<
			"Default value is: " << voltage_incr << " V" << std::endl <<
			"voltage increment = ";
        std::cin >> voltage_incr;
    }
}



/*************************************************************************
 * scan-- Private
 * Description-- scan the Vortex and record the data
 * Input-- voltage_vector, a vector where the Vortex voltages are to be
 *             stored.
 *         DAQ_vector, a vector where the spectrum data is to be stored.
 *         usb1408fs, address of the usb object.
 * Output-- None
 * Return-- bool. False if the scan was successful.
 *************************************************************************/
bool GETLOCK::scan (std::vector <double>& voltage_vector, 
					std::vector <double>& DAQ_vector, USB1408FS &usb1408fs)
{
	double voltage = start_voltage;

	while(voltage <= end_voltage) {
		//set the output voltage to the back of the vortex
		usb1408fs.set_output_voltage(usb_output_channel, (float) voltage); 

		//record DAQ output voltage
		voltage_vector.push_back(voltage); 

		//take data
		DAQ_vector.push_back(usb1408fs.read_input_channel(usb_input_channel)); 

		// change the frequency
		voltage = voltage + voltage_incr;

		//wait for the DAQ to settle. spec'd rate is 10 KS/s
		Sleep(25); 
	}

	// Check the success of the scan
	if (voltage_vector.size() == 0 || DAQ_vector.size() == 0) {
		std::cerr <<std::endl << "Error in GETLOCK::scan-- no data received" <<
			std::endl;
		return (1);
	}

	return (0);
}



/*************************************************************************
 * plot-- Private
 * Description-- plot the data in Matlab
 * Input-- voltage_vector, a vector of Vortex voltages.
 *         DAQ_vector, a vector of the spectrum data.
 *         matlabplotter, address of the Matlab object.
 * Output-- Matlab plot
 *************************************************************************/
void GETLOCK::plot(std::vector <double>& voltage_vector, 
				   std::vector <double>& DAQ_vector, 
				   MATLABPLOTTER &matlabplotter)
{
	bool save_data = true;

	matlabplotter.plotfreqscan(voltage_vector, DAQ_vector, true);

	std::cout << "Do you want to save the data (1/0)?";
    std::cin >> save_data;

	matlabplotter.savedata(save_data);
}


/*************************************************************************
 * plot-- Private
 * Description-- plot the data in Matlab
 * Input-- voltage_vector, a vector of Vortex voltages.
 *         DAQ_vector, a vector of the spectrum data.
 *         voltageSB_vector, a vector of the sideband bounds and the 
 *             calculated peak. To be plotted as points
 *         DAQSB_vector, a vector of the correspond spectrum data.
 *         matlabplotter, address of the Matlab object.
 * Output-- Matlab plot
 *************************************************************************/
void GETLOCK::plot(std::vector <double>& voltage_vector, 
				   std::vector <double>& DAQ_vector, 
				   std::vector <double>& voltageSB_vector, 
				   std::vector <double>& DAQSB_vector, 
				   MATLABPLOTTER &matlabplotter)
{

	bool save_data = true;

	
	matlabplotter.plotfreqscan(voltage_vector, DAQ_vector, false);
	matlabplotter.plotlockpoints(voltageSB_vector,DAQSB_vector);

	std::cout << "Do you want to save the data (1/0)?";
    std::cin >> save_data;

	matlabplotter.savedata(save_data);
}



/*************************************************************************
 * findCoolingPeak-- Private
 * Description-- finds the possible minima in the data, selects the two
 *             lowest, then find the maximum in between them. This maximum
 *             ought to be the Rb 85 cooling transition.
 * Input-- voltage_vector, a vector of laser voltages.
 *         DAQ_vector, a vector of corresponding spectrum voltages.
 *         start, the position to start looking for the peak.
 *         end, the position to stop looking for the peak.
 * Return-- the Vortex voltage of the minimum.
 *************************************************************************/
double GETLOCK::findCoolingPeak(std::vector <double>& voltage_vector, 
								std::vector <double>& DAQ_vector,
								int start, int end)
{
	int i;
	std::vector <int> minPositions;
	int oldTempMinPos = 0;
	int tempMinPos;
	int minPosSmaller;
	int minPosLarger;
	int count = 1;
	double windowV = .08 * GHzToV;
	int window = 
		(int) ceil(windowV / fabs(voltage_vector.at(1)-voltage_vector.at(0)));

	std::cout << "Window: " << window << std::endl;

	// Find all possible minima
	for (i = start; i < end - window; i++)
	{
		tempMinPos = findGlobalMin(voltage_vector, DAQ_vector, i, i + window);
		if (tempMinPos == oldTempMinPos)
		{
			count++;
			if (count == window) {
				count = 1;
				minPositions.push_back(tempMinPos);
			}
		}
		else {
			oldTempMinPos = tempMinPos;
			count = 1;
		}
	}

	if (minPositions.size() <= 1) {
		std::cerr << "Error in findCoolingPeak-- not enough minima found" <<
			std::endl;
		return (0);
	}

	// Select the two lowest minima. 
	// NOTE: this function alters minPostions!
	getTwoLowestMinima(voltage_vector, DAQ_vector, minPositions,
		&minPosSmaller, &minPosLarger);

	//std::cerr << voltage_vector.at(minPosSmaller) << std::endl;
	//std::cerr << voltage_vector.at(minPosLarger) << std::endl;

	// Return the maximum between the two minima
	return (voltage_vector.at(findGlobalMax(voltage_vector, DAQ_vector,
		minPosSmaller, minPosLarger)));
}



/*************************************************************************
 * findGlobalMin-- Private
 * Description-- return the Vortex voltage corresponding to the minimum 
 *             of the spectrum.
 * Input-- voltage_vector, a vector of laser voltages.
 *         DAQ_vector, a vector of corresponding spectrum voltages.
 *         start, the position to start searching for the minimum.
 *         end, the position to stop searching for the minimum.
 * Return-- the position of the minimum.
 *************************************************************************/
int GETLOCK::findGlobalMin(std::vector <double>& voltage_vector, 
						   std::vector <double>& DAQ_vector, 
						   int start, int end)
{
	unsigned int i;
	unsigned int tempMinPos = start;
	double tempMin = DAQ_vector.at(tempMinPos);

	for (i = start; i < (unsigned) end; i++)
	{
		if (DAQ_vector.at(i) < tempMin)
		{
			tempMin = DAQ_vector.at(i);
			tempMinPos = i;
		}
	}

	return (tempMinPos);
}



/*************************************************************************
 * getTwoLowestMinima-- Private
 * Description-- given a list of minima, finds the two lowest
 * Input-- voltage_vector, a vector of laser voltages.
 *         DAQ_vector, a vector of corresponding spectrum voltages.
 *         minPositions, a vector of the positions of minima
 *         minPosSmaller_p, a pointer to the place where the smaller
 *             position of the two minima should be stored.
 *         minPosLarger_p, a pointer to the place where the larger
 *             postion of the two minima should be stored.
 *************************************************************************/
void GETLOCK::getTwoLowestMinima (std::vector <double>& voltage_vector, 
								  std::vector <double>& DAQ_vector, 
								  std::vector <int>& minPositions, 
								  int* minPosSmaller_p, int* minPosLarger_p)
{
	unsigned int i;
	int tempMinPos;
	int tempMinPos1;
	int tempMinPos2;
	std::vector <double> minDAQ_vector;
	std::vector <double> minVoltage_vector;

	if (minPositions.size() <= 1) {
		std::cerr << "Error in getTwoLowestMinima-- not enough minima found" << 
			std::endl;
		return;
	}

	// Load the Vortex voltages and the DAQ voltages of the minima
	for (i = 0; i < minPositions.size(); i++)
	{
		minDAQ_vector.push_back(DAQ_vector.at(minPositions.at(i)));
		minVoltage_vector.push_back(voltage_vector.at(minPositions.at(i)));
	}

	// Find the lowest minimum
	tempMinPos = findGlobalMin(minVoltage_vector, minDAQ_vector,
		0, minVoltage_vector.size());
	tempMinPos1 = minPositions.at(tempMinPos);

	// Erase the lowest minimum
	minVoltage_vector.erase(minVoltage_vector.begin() + tempMinPos);
	minDAQ_vector.erase(minDAQ_vector.begin() + tempMinPos);
	minPositions.erase(minPositions.begin() + tempMinPos);

	// Find the second-lowest minimum
	tempMinPos2 = minPositions.at(findGlobalMin(minVoltage_vector, minDAQ_vector,
		0, minVoltage_vector.size()));

	// Sort the positions
	if(tempMinPos1 > tempMinPos2) {
		tempMinPos = tempMinPos2;
		tempMinPos2 = tempMinPos1;
		tempMinPos1 = tempMinPos;
	}

	*minPosSmaller_p = tempMinPos1;
	*minPosLarger_p = tempMinPos2;
}



/*************************************************************************
 * findGlobalMax-- Private
 * Description-- find the Vortex voltage corresponding to the greatest 
 *             element of the spectrum within a given range.
 * Input-- voltage_vector, a vector of Vortex voltages.
 *         DAQ_vector, a vector of corresponding spectrum voltages.
 *         start, the position to start searching for the maximum.
 *         end, the position to stop searching for the maximum.
 * Return-- the position of the maximum.
 *************************************************************************/
int GETLOCK::findGlobalMax(std::vector <double>& voltage_vector, 
						   std::vector <double>& DAQ_vector, 
						   int start, int end)
{
	unsigned int i;
	unsigned int tempMaxPos = start;
	double tempMax = DAQ_vector.at(tempMaxPos);

	for (i = start; i < (unsigned) end; i++)
	{
		if (DAQ_vector.at(i) > tempMax)
		{
			tempMax = DAQ_vector.at(i);
			tempMaxPos = i;
		}
	}

	return (tempMaxPos);
}



/*************************************************************************
 * position-- Private
 * Description-- Finds the first element in the vector that most nearly 
 *         matches the given element.
 * Input-- myVector, a vector of elements to be searched
 *         element, the element to be looked for.
 * Return-- the position in myVector of the element.
 * NOTE-- this function works only when myVector increases or decreases
 *         monotonically. Identitcal to WHICHLOCK::position
 *************************************************************************/
int GETLOCK::position(std::vector <double>& myVector, double element)
{
	double oldDiff = fabs(myVector.at(0)-element);
	double newDiff = fabs(myVector.at(1)-element);
	int i = 1;	

	while(newDiff < oldDiff && i < (signed int) myVector.size() - 1)
	{
		i++;
		oldDiff = newDiff;
		newDiff = fabs(myVector.at(i)-element);
	}

	if(i == myVector.size() + 1){
		std::cout << "WHICHLOCK::position-- "
			<< "Warning: nearest value at end of vector" << std::endl;
	}

	return (i - 1);
}




/*************************************************************************
 * findSidebandPeak-- Private
 * Description-- return the Vortex voltage corresponding to the greatest 
 *             element of the spectrum within a given range.
 * Input-- voltage_vector, a vector of Vortex voltages.
 *         DAQ_vector, a vector of corresponding spectrum voltages.
 *         start, the position to start searching for the maximum.
 *         end, the position to stop searching for the maximum.
 * Return-- the position of the maximum.
 *************************************************************************/
double GETLOCK::findSidebandPeak(std::vector <double>& voltage_vector, std::vector <double>& DAQ_vector, int start, int end)
{

	int i;
	std::vector <int> maxPositions;
	int oldTempMaxPos = 0;
	int tempMaxPos;
	int count = 1;
	double windowV = .1 * GHzToV; // should be roughly width of base of Lorentzian
	int window = (int) ceil(windowV / fabs(voltage_vector.at(1)-voltage_vector.at(0)));

	for (i = start; i < end; i++)
	{
		tempMaxPos = findGlobalMax(voltage_vector, DAQ_vector, i, i + window);
		if (tempMaxPos == oldTempMaxPos)
		{
			count++;
			if (count == window) {
				count = 1;
				maxPositions.push_back(tempMaxPos);
			}
		}
		else {
			oldTempMaxPos = tempMaxPos;
			count = 1;
		}
	}

	if (maxPositions.size() == 0) {
		std::cerr << "Error in findCoolingPeak-- no maxima found" << std::endl;
		return (0);
	}

	return (voltage_vector.at(derivativeTest(voltage_vector, DAQ_vector, maxPositions, window)));
}



/*************************************************************************
 * derivativeTest-- Private
 * Description-- selects the peak with the largest rise.
 * Input-- voltage_vector, a vector of Vortex voltages.
 *         DAQ_vector, a vector of corresponding spectrum voltages.
 *         maxPositions, the positions of the minima
 *         bigWindow, the window used in findSidebandPeak.
 * Return-- 
 *************************************************************************/
int GETLOCK::derivativeTest(std::vector <double>& voltage_vector, std::vector <double>& DAQ_vector, std::vector <int>& maxPositions, int bigWindow)
{
	int end = maxPositions.size();
	int i;
	int j;
	int window = (int) ceil((double) bigWindow / 2); //ensures we don't go out of range with the sides array
	int sides[3] = {-window, 0, window};
	int posList[3];
	double leftRise;
	double rightRise;
	double totalRise;
	double oldTotalRise = 0;
	int tempPos = 0;


	for (i = 0; i < end; i++)
	{
		for (j = 0; j < 3; j++)
		{
			// Find the position a given distance away from the expected max.
			posList[j] = maxPositions.at(i) + sides[j];
		}
		leftRise = DAQ_vector.at(posList[1]) - DAQ_vector.at(posList[0]);
		rightRise = DAQ_vector.at(posList[1]) - DAQ_vector.at(posList[2]);
		totalRise = leftRise + rightRise;
		if (totalRise > oldTotalRise){
			tempPos = i;
			oldTotalRise = totalRise;
		}
	}

	return (maxPositions.at(tempPos));
}

