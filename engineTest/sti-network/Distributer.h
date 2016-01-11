

/////TEMP/////////





template<class ID, class T>
class Distributer : public AbstractDistributer<ID>
{
	typedef boost::shared_ptr<T> T_ptr;

	typedef Collector<ID, T> CollectorT;
	typedef boost::shared_ptr<CollectorT> CollectorT_ptr;

	typedef boost::shared_ptr< CollectorNode<ID, T> > CollectorNode_ptr;

	bool addCollectorNode(const ID& id, const CollectorNode_ptr& node)
	{
		CollectorT_ptr collector;
		if(addNode(id, node) && node->getCollector(collector) && addCollector(collector)) {
			return true;
		}
		else {
			remove(id);
			return false;
		}
	}
};
