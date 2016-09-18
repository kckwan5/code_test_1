#include <unordered_map>
#include <mutex>
#include <queue>
#include <memory>

namespace UnitTest
{
	class Tester;
}

class OrderBookInterface
{
public:
	typedef uint64_t qty_type;
        typedef uint64_t stock_id_type;
        typedef uint64_t client_id_type;
        typedef uint64_t order_id_type;
        typedef uint64_t trader_id_type;

        enum ORDER_SIDE
        {
                BUY_ORDER,
                SELL_ORDER
        };

        struct Order
        {

                order_id_type m_orderId;
                qty_type m_qty;
                qty_type m_remainQty;
                client_id_type m_clientId;
                trader_id_type m_traderId;
                stock_id_type m_stockId;
                ORDER_SIDE m_side;

        };

	virtual void addOrder(Order const &order,std::vector<Order> &matchedOrder)=0;
};


class OrderBook : public OrderBookInterface
{
public:
	friend class UnitTest::Tester;
private:
        struct OrderQueue
        {
		OrderQueue():m_side(BUY_ORDER),m_totalRemainQty(0),m_queue(){}

                std::mutex m_mutex;	// so concurrent order of same instrument will not have raise condition
                std::queue<Order> m_queue; // store pending order for current side
		ORDER_SIDE m_side; // current side of the queue orders, since there is no price all opposite order will immediately match until one side's order remain
		qty_type m_totalRemainQty; // total remain quantity of pending orders
        };

public:
	void addOrder(Order const &order,std::vector<Order> &matchedOrder);

	OrderBook();
	virtual ~OrderBook();

private:

	typedef std::unordered_map<stock_id_type,std::shared_ptr<OrderQueue> > order_queues_type;

	order_queues_type m_order_queues;
};

class OrderBookFactory
{
public:
	static std::auto_ptr<OrderBookInterface> CreateOrderBook()
	{
		return std::auto_ptr<OrderBookInterface>(new OrderBook());
	}
};
/*
class Tester
{
        public:
                static void dumpQueue(OrderBook const &orderbook, OrderBookInterface::stock_id_type stockId, bool &recordFound,std::queue<OrderBookInterface::Order> &queue,OrderBookInterface::qty_type &remainQty)
                {
                        OrderBook::order_queues_type::const_iterator it=orderbook.m_order_queues.find(stockId);
                }
};
*/
