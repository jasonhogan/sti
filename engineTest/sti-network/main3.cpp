
#include "DualDistributer.h"
#include "DeviceID.h"
#include "RefConverter.h"
#include "DeviceInterface.h"

using STI::Utils::RefConverter;

class LocalDevice : public STI::Device::DeviceInterface
{
};
class NetworkDevice : public STI::Device::DeviceInterface
{
};

class DeviceConverter : public RefConverter<LocalDevice, NetworkDevice>
{
	virtual bool convert(const RefConverter<LocalDevice, NetworkDevice>::T1_ptr&, RefConverter<LocalDevice, NetworkDevice>::T2_ptr&)
	{
		return false;
	}
	virtual bool convert(const RefConverter<LocalDevice, NetworkDevice>::T2_ptr&, RefConverter<LocalDevice, NetworkDevice>::T1_ptr&)
	{
		return false;
	}
};

int main3(int argc, char **argv)
{
	typedef boost::shared_ptr<DeviceConverter> DeviceConverter_ptr;
	typedef STI::Utils::Distributer<STI::Device::DeviceID, LocalDevice, STI::Device::DeviceInterface> LocalDistributer;
	typedef boost::shared_ptr<LocalDistributer> LocalDistributer_ptr;
	typedef STI::Utils::Distributer<STI::Device::DeviceID, NetworkDevice, STI::Device::DeviceInterface> NetworkDistributer;
	typedef boost::shared_ptr<NetworkDistributer> NetworkDistributer_ptr;

	DeviceConverter_ptr converter(new DeviceConverter());
	LocalDistributer_ptr dist1(new LocalDistributer());
	NetworkDistributer_ptr dist2(new NetworkDistributer());

	STI::Utils::DualDistributer<STI::Device::DeviceID, LocalDevice, NetworkDevice, STI::Device::DeviceInterface> 
		distributer(dist1, dist2, converter);

	distributer.distribute();
	distributer.cleanup();
	distributer.refresh();

	return 0;
}
//
//template<typename ID, typename T>
//class CollectorNode : public T, public Collector<ID, T>
//{
//};






