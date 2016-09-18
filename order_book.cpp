#include "order_book.h"
#include <thread>
#include <iostream>

OrderBook::OrderBook()
{

}

OrderBook::~OrderBook()
{
}

void OrderBook::addOrder(Order const &order,std::vector<Order> &completedOrder)
{
	order_queues_type::iterator itQueue=m_order_queues.find(order.m_stockId);

	if(itQueue==m_order_queues.end())
	{
		static std::mutex createMutex;
		std::lock_guard<std::mutex> createGuard(createMutex); // to prevent raise condition when creating the OrderQueue object
		itQueue=m_order_queues.find(order.m_stockId);
		if(itQueue==m_order_queues.end())
		{
			m_order_queues[order.m_stockId]=std::auto_ptr<OrderQueue>(new OrderQueue());
			itQueue=m_order_queues.find(order.m_stockId);
		}
	}

	std::lock_guard<std::mutex> queueGuard(itQueue->second->m_mutex);

	if(order.m_side!=itQueue->second->m_side)
	{
		uint64_t matchedQty=0;
		if(itQueue->second->m_totalRemainQty>=order.m_qty)
		{
			matchedQty=order.m_qty;
			completedOrder.push_back(order);
			completedOrder.back().m_remainQty=0;
			
			itQueue->second->m_totalRemainQty-=matchedQty;
			while(matchedQty>0)
			{
				Order &pendingOrder=itQueue->second->m_queue.front();
				if(pendingOrder.m_remainQty>matchedQty)
				{
					pendingOrder.m_remainQty-=matchedQty;
					matchedQty=0;
				}
				else
				{
					matchedQty-=pendingOrder.m_remainQty;
					completedOrder.push_back(pendingOrder);
					completedOrder.back().m_remainQty=0;
					itQueue->second->m_queue.pop();
				}
			}
		}
		else
		{
			matchedQty=itQueue->second->m_totalRemainQty;
			uint64_t remainQtyForNewOrder=order.m_qty-itQueue->second->m_totalRemainQty;
			while(!itQueue->second->m_queue.empty())
			{	
				matchedQty-=itQueue->second->m_queue.front().m_remainQty;
				completedOrder.push_back(itQueue->second->m_queue.front());
				completedOrder.back().m_remainQty=0;
				itQueue->second->m_queue.pop();
			}
			itQueue->second->m_queue.push(order);
			itQueue->second->m_queue.back().m_remainQty=remainQtyForNewOrder;
			itQueue->second->m_totalRemainQty=remainQtyForNewOrder;
			itQueue->second->m_side=order.m_side;
		}
		
	}
	else
	{
		itQueue->second->m_queue.push(order);
		itQueue->second->m_queue.back().m_remainQty=order.m_qty;
		itQueue->second->m_totalRemainQty+=order.m_qty;
	}
}

