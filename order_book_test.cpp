#include "order_book.h"
#include <iostream>
#include <string>

namespace UnitTest
{
	class Tester
	{
	public:
		static void dumpQueue(OrderBook const &orderbook, OrderBookInterface::stock_id_type stockId, bool &recordFound,std::queue<OrderBookInterface::Order> &queue,OrderBookInterface::qty_type &remainQty
				      ,OrderBookInterface::ORDER_SIDE &queueSide)
		{
			OrderBook::order_queues_type::const_iterator it=orderbook.m_order_queues.find(stockId);
			if(it==orderbook.m_order_queues.end())
			{
				recordFound=false;
				return;
			}
			recordFound=true;
			queue=it->second->m_queue;
			remainQty=it->second->m_totalRemainQty;
			queueSide=it->second->m_side;
			
		}
		
	};

	void ReportInfo(std::string const &message)
	{
		std::cout << "Info: " << message << std::endl;
	}	

	void ReportError(std::string const &message)
	{
		std::cout << "Error: " << message << std::endl;
	}

};

namespace
{
	enum
        {
                TRADER1=1
                ,TRADER2=2
		,TRADER3=3
		,TRADER4=4
        };

	enum
	{
		STOCK1=1
		,STOCK2=2
		,STOCK3=3
	};

	std::string to_string_bool(bool value)
	{
		return value?"true":"false";
	}


	bool CheckQueue(std::string const &suiteCaseOrder
			,OrderBook const &orderbook
			,OrderBookInterface::stock_id_type stockId
			,bool expectRecordFound
			,OrderBookInterface::qty_type expectRemainQty
                        ,OrderBookInterface::ORDER_SIDE expectQueueSide
			,std::queue<OrderBookInterface::Order> &queue)
	{
		bool recordFound;
                OrderBookInterface::qty_type remainQty;
                OrderBookInterface::ORDER_SIDE queueSide;
                UnitTest::Tester::dumpQueue(orderbook,stockId,recordFound,queue,remainQty,queueSide);

                if(recordFound!=expectRecordFound)
                {
                        UnitTest::ReportError(suiteCaseOrder+": OrderQueue found unexpected "+std::to_string(recordFound)+"!="+std::to_string(expectRecordFound));
                        return false;
                }
		else
		{
			UnitTest::ReportInfo(suiteCaseOrder+": OrderQueue found expected");
		}
		
		if(expectRecordFound==false) return true;



                if(remainQty!=expectRemainQty)
                {
                        UnitTest::ReportError(suiteCaseOrder+": TotalRemainQty unexpected "+std::to_string(remainQty)+"!="+std::to_string(expectRemainQty));
                        return false;
                }
		else
		{
			UnitTest::ReportInfo(suiteCaseOrder+": TotalRemainQty expected ");
		}

                if(queueSide!=expectQueueSide)
                {
                        UnitTest::ReportError(suiteCaseOrder+": queue side unexpected"+std::to_string(queueSide)+"!="+std::to_string(expectQueueSide));
                        return false;
                }
		else
		{
			UnitTest::ReportInfo(suiteCaseOrder+": queue side expected");
		}
		
		return true;
	}
	

	bool CheckOrder(std::string const &suiteCaseOrder,OrderBookInterface::Order const &order,OrderBookInterface::order_id_type orderId,OrderBookInterface::qty_type qty,OrderBookInterface::qty_type remainQty,OrderBookInterface::trader_id_type traderId,OrderBookInterface::stock_id_type stockId,OrderBookInterface::ORDER_SIDE side)
	{
		if(order.m_orderId!=orderId)
		{
			UnitTest::ReportError(std::string(suiteCaseOrder)+": orderId unmatch "+std::to_string(order.m_orderId)+"!="+std::to_string(orderId));
	                return false;
		}
		else
		{
			UnitTest::ReportInfo(std::string(suiteCaseOrder)+": orderId match");
		}

		if(order.m_qty!=qty)
                {
                        UnitTest::ReportError(std::string(suiteCaseOrder)+": qty unmatch "+std::to_string(order.m_qty)+"!="+std::to_string(qty));
                        return false;
                }
                else
                {
                        UnitTest::ReportInfo(std::string(suiteCaseOrder)+": qty match");
                }
		
		if(order.m_remainQty!=remainQty)
                {
                        UnitTest::ReportError(std::string(suiteCaseOrder)+": remainQty unmatch "+std::to_string(order.m_remainQty)+"!="+std::to_string(remainQty));
                        return false;
                }
                else
                {
                        UnitTest::ReportInfo(std::string(suiteCaseOrder)+": remainQty match");
                }

		if(order.m_traderId!=traderId)
                {
                        UnitTest::ReportError(std::string(suiteCaseOrder)+": traderId unmatch "+std::to_string(order.m_traderId)+"!="+std::to_string(traderId));
                        return false;
                }
                else
                {
                        UnitTest::ReportInfo(std::string(suiteCaseOrder)+": traderId match");
                }

		if(order.m_stockId!=stockId)
                {
                        UnitTest::ReportError(std::string(suiteCaseOrder)+": stockId unmatch "+std::to_string(order.m_stockId)+"!="+std::to_string(stockId));
                        return false;
                }
                else
                {
                        UnitTest::ReportInfo(std::string(suiteCaseOrder)+": stockId match");
                }


		return true;
	}
	
	template<typename T>
	bool CheckSize(std::string const &desc,T const &container,size_t size)
	{
		if(container.size()!=size)
		{
			UnitTest::ReportError(std::string(desc)+": container size unmatch "+std::to_string(container.size())+"!="+std::to_string(size));
			return false;
		}
		else
		{
			UnitTest::ReportInfo(std::string(desc)+": container size match ");
		}
		return true;
	}

	void AddOrder(OrderBookInterface &orderbook
			,OrderBookInterface::order_id_type orderId
                        ,OrderBookInterface::stock_id_type stockId
                        ,OrderBookInterface::qty_type qty
                        ,OrderBookInterface::ORDER_SIDE side
			,OrderBookInterface::trader_id_type traderId
			,std::vector<OrderBookInterface::Order> &matchedOrder)
	{
		OrderBookInterface::Order order;
                order.m_orderId=orderId;
                order.m_qty=qty;
                order.m_traderId=traderId;
                order.m_stockId=stockId;
                order.m_side=side;

		orderbook.addOrder(order,matchedOrder);
	}

}

void TestSuite1()
{

	UnitTest::ReportInfo("Suite1 Case1: Create Object and check type correctness");
	std::auto_ptr<OrderBookInterface> orderbookInt=OrderBookFactory::CreateOrderBook();
	
	OrderBook *orderbook=dynamic_cast<OrderBook*>(orderbookInt.get());
	if(orderbook==NULL)
	{
		UnitTest::ReportError("Suite1 Case1: Instance with type OrderBook was not created correctly");
		return;
	}
	
	{
		std::vector<OrderBookInterface::Order> matchedOrder;
		AddOrder(*orderbookInt,1,STOCK1,100,OrderBookInterface::BUY_ORDER,TRADER1,matchedOrder);

		if(!CheckSize<std::vector<OrderBookInterface::Order> >("Suite1 Case1",matchedOrder,0)) return;

		std::queue<OrderBookInterface::Order> queue;
		if(CheckQueue("Suite1 Case1 Check2",*orderbook,STOCK1,true,100,OrderBookInterface::BUY_ORDER,queue)==false) return;

		if(!CheckSize<std::queue<OrderBookInterface::Order> >("Suite1 Case1 Check3",queue,1)) return;

		OrderBookInterface::Order &firstQueue=queue.front();
                if(CheckOrder("Suite1 Case1 Check4",firstQueue,1,100,100,TRADER1,STOCK1,OrderBookInterface::BUY_ORDER)==false) return;

		if(CheckQueue("Suite1 Case1 Check5",*orderbook,STOCK2,false,0,OrderBookInterface::BUY_ORDER,queue)==false) return;
		
	}

	{
		std::vector<OrderBookInterface::Order> matchedOrder;
		AddOrder(*orderbookInt,2,STOCK2,200,OrderBookInterface::BUY_ORDER,TRADER1,matchedOrder);

                if(!CheckSize<std::vector<OrderBookInterface::Order> >("Suite1 Case2",matchedOrder,0)) return;
                

                std::queue<OrderBookInterface::Order> queue;
                if(CheckQueue("Suite1 Case2 Check2",*orderbook,STOCK1,true,100,OrderBookInterface::BUY_ORDER,queue)==false) return;

		if(!CheckSize<std::queue<OrderBookInterface::Order> >("Suite1 Case2 Check3",queue,1)) return;

                OrderBookInterface::Order &queueOrder1=queue.front();
                if(CheckOrder("Suite1 Case2 Check4",queueOrder1,1,100,100,TRADER1,STOCK1,OrderBookInterface::BUY_ORDER)==false) return;

                if(CheckQueue("Suite1 Case2 Check5",*orderbook,STOCK2,true,200,OrderBookInterface::BUY_ORDER,queue)==false) return;

		if(!CheckSize<std::queue<OrderBookInterface::Order> >("Suite1 Case2 Check6",queue,1)) return;
		
		OrderBookInterface::Order &queueOrder2=queue.front();
                if(CheckOrder("Suite1 Case2 Check7",queueOrder2,2,200,200,TRADER1,STOCK2,OrderBookInterface::BUY_ORDER)==false) return;


        }

	{
		std::vector<OrderBookInterface::Order> matchedOrder;
                AddOrder(*orderbookInt,3,STOCK1,100,OrderBookInterface::SELL_ORDER,TRADER3,matchedOrder);

                if(!CheckSize<std::vector<OrderBookInterface::Order> >("Suite1 Case3",matchedOrder,2)) return;
		
		OrderBookInterface::Order &matchedOrder1=matchedOrder[0];
		if(CheckOrder("Suite1 Case3 Check2",matchedOrder1,3,100,0,TRADER3,STOCK1,OrderBookInterface::SELL_ORDER)==false) return;

		OrderBookInterface::Order &matchedOrder2=matchedOrder[1];
                if(CheckOrder("Suite1 Case3 Check3",matchedOrder2,1,100,0,TRADER1,STOCK1,OrderBookInterface::BUY_ORDER)==false) return;

		

                std::queue<OrderBookInterface::Order> queue;
                if(CheckQueue("Suite1 Case3 Check5",*orderbook,STOCK1,true,0,OrderBookInterface::BUY_ORDER,queue)==false) return;

		if(!CheckSize<std::queue<OrderBookInterface::Order> >("Suite1 Case3 Check6",queue,0)) return;

                if(CheckQueue("Suite1 Case3 Check7",*orderbook,STOCK2,true,200,OrderBookInterface::BUY_ORDER,queue)==false) return;

                OrderBookInterface::Order &queueOrder2=queue.front();
                if(CheckOrder("Suite1 Case3 Check8",queueOrder2,2,200,200,TRADER1,STOCK2,OrderBookInterface::BUY_ORDER)==false) return;


        }

	{
                std::vector<OrderBookInterface::Order> matchedOrder;
                AddOrder(*orderbookInt,4,STOCK1,300,OrderBookInterface::SELL_ORDER,TRADER3,matchedOrder);

		if(!CheckSize<std::vector<OrderBookInterface::Order> >("Suite1 Case4",matchedOrder,0)) return;

                std::queue<OrderBookInterface::Order> queue;
                if(CheckQueue("Suite1 Case4 Check2",*orderbook,STOCK1,true,300,OrderBookInterface::SELL_ORDER,queue)==false) return;

		if(!CheckSize<std::queue<OrderBookInterface::Order> >("Suite1 Case4 Check3",queue,1)) return;

		OrderBookInterface::Order &queueOrder=queue.front();
                if(CheckOrder("Suite1 Case4 Check4",queueOrder,4,300,300,TRADER3,STOCK1,OrderBookInterface::SELL_ORDER)==false) return;

        }

	{
                std::vector<OrderBookInterface::Order> matchedOrder;
                AddOrder(*orderbookInt,5,STOCK1,100,OrderBookInterface::BUY_ORDER,TRADER1,matchedOrder);

		if(!CheckSize<std::vector<OrderBookInterface::Order> >("Suite1 Case5",matchedOrder,1)) return;

		OrderBookInterface::Order &matchedOrder1=matchedOrder[0];
		if(CheckOrder("Suite1 Case5",matchedOrder1,5,100,0,TRADER1,STOCK1,OrderBookInterface::BUY_ORDER)==false) return;


                std::queue<OrderBookInterface::Order> queue;
                if(CheckQueue("Suite1 Case5 Check2",*orderbook,STOCK1,true,200,OrderBookInterface::SELL_ORDER,queue)==false) return;


                OrderBookInterface::Order &queueOrder=queue.front();
                if(CheckOrder("Suite1 Case4 Check2",queueOrder,4,300,200,TRADER3,STOCK1,OrderBookInterface::SELL_ORDER)==false) return;

        }

	{
                std::vector<OrderBookInterface::Order> matchedOrder;
                AddOrder(*orderbookInt,6,STOCK1,200,OrderBookInterface::BUY_ORDER,TRADER1,matchedOrder);

                if(matchedOrder.size()!=2)
                {
                        UnitTest::ReportError("Suite1 Case6: matched order unexpected");
                        return;
                }

                OrderBookInterface::Order &matchedOrder1=matchedOrder[0];
                if(CheckOrder("Suite1 Case6",matchedOrder1,6,200,0,TRADER1,STOCK1,OrderBookInterface::BUY_ORDER)==false) return;

		OrderBookInterface::Order &matchedOrder2=matchedOrder[1];
                if(CheckOrder("Suite1 Case6 Check2",matchedOrder2,4,300,0,TRADER3,STOCK1,OrderBookInterface::SELL_ORDER)==false) return;



                std::queue<OrderBookInterface::Order> queue;
                if(CheckQueue("Suite1 Case6 Check3",*orderbook,STOCK1,true,0,OrderBookInterface::SELL_ORDER,queue)==false) return;

		if(!CheckSize<std::queue<OrderBookInterface::Order> >("Suite1 Case6 Check4",queue,0)) return;

        }

	
}

void TestSuite2()
{

        UnitTest::ReportInfo("Suite2 Case1: Create Object and check type correctness");
        std::auto_ptr<OrderBookInterface> orderbookInt=OrderBookFactory::CreateOrderBook();
        OrderBook *orderbook=dynamic_cast<OrderBook*>(orderbookInt.get());
        if(orderbook==NULL)
        {
                UnitTest::ReportError("Suite2 Case1: Instance with type OrderBook was not created correctly");
                return;
        }

        {
                std::vector<OrderBookInterface::Order> matchedOrder;
                AddOrder(*orderbookInt,1,STOCK1,100,OrderBookInterface::BUY_ORDER,TRADER1,matchedOrder);

		AddOrder(*orderbookInt,2,STOCK1,200,OrderBookInterface::SELL_ORDER,TRADER2,matchedOrder);
		if(!CheckSize<std::vector<OrderBookInterface::Order> >("Suite2 Case1",matchedOrder,1)) return;

		OrderBookInterface::Order &matchedOrder1=matchedOrder[0];
                if(CheckOrder("Suite2 Case1 Check2",matchedOrder1,1,100,0,TRADER1,STOCK1,OrderBookInterface::BUY_ORDER)==false) return;

		std::queue<OrderBookInterface::Order> queue;
                if(CheckQueue("Suite2 Case1 Check3",*orderbook,STOCK1,true,100,OrderBookInterface::SELL_ORDER,queue)==false) return;

		if(!CheckSize<std::queue<OrderBookInterface::Order> >("Suite2 Case1 Check4",queue,1)) return;
		
		OrderBookInterface::Order &queueOrder=queue.front();
		if(CheckOrder("Suite2 Case1 Check4",queueOrder,2,200,100,TRADER2,STOCK1,OrderBookInterface::SELL_ORDER)==false) return;
        }
}

void TestSuite3()
{

        UnitTest::ReportInfo("Suite3 Case1: Create Object and check type correctness");
        std::auto_ptr<OrderBookInterface> orderbookInt=OrderBookFactory::CreateOrderBook();
        OrderBook *orderbook=dynamic_cast<OrderBook*>(orderbookInt.get());
        if(orderbook==NULL)
        {
                UnitTest::ReportError("Suite3 Case1: Instance with type OrderBook was not created correctly");
                return;
        }

        {
                std::vector<OrderBookInterface::Order> matchedOrder;
                AddOrder(*orderbookInt,1,STOCK1,100,OrderBookInterface::BUY_ORDER,TRADER1,matchedOrder);

                AddOrder(*orderbookInt,2,STOCK1,200,OrderBookInterface::BUY_ORDER,TRADER2,matchedOrder);

		AddOrder(*orderbookInt,3,STOCK1,300,OrderBookInterface::BUY_ORDER,TRADER3,matchedOrder);


                if(!CheckSize<std::vector<OrderBookInterface::Order> >("Suite3 Case1 Check2",matchedOrder,0)) return;


                std::queue<OrderBookInterface::Order> queue;
                if(CheckQueue("Suite3 Case1 Check3",*orderbook,STOCK1,true,600,OrderBookInterface::BUY_ORDER,queue)==false) return;

                if(!CheckSize<std::queue<OrderBookInterface::Order> >("Suite3 Case1 Check4",queue,3)) return;

                OrderBookInterface::Order &queueOrder1=queue.front();
                if(CheckOrder("Suite3 Case1 Check4",queueOrder1,1,100,100,TRADER1,STOCK1,OrderBookInterface::BUY_ORDER)==false) return;

		queue.pop();
		OrderBookInterface::Order &queueOrder2=queue.front();
                if(CheckOrder("Suite3 Case1 Check5",queueOrder2,2,200,200,TRADER2,STOCK1,OrderBookInterface::BUY_ORDER)==false) return;

		queue.pop();
		OrderBookInterface::Order &queueOrder3=queue.front();
                if(CheckOrder("Suite3 Case1 Check6",queueOrder3,3,300,300,TRADER3,STOCK1,OrderBookInterface::BUY_ORDER)==false) return;


        }
	
	{
                std::vector<OrderBookInterface::Order> matchedOrder;
                AddOrder(*orderbookInt,4,STOCK1,600,OrderBookInterface::SELL_ORDER,TRADER4,matchedOrder);



                if(!CheckSize<std::vector<OrderBookInterface::Order> >("Suite3 Case2",matchedOrder,4)) return;

		OrderBookInterface::Order &matchedOrder1=matchedOrder[0];
                if(CheckOrder("Suite3 Case2 Check2",matchedOrder1,4,600,0,TRADER4,STOCK1,OrderBookInterface::SELL_ORDER)==false) return;

		OrderBookInterface::Order &matchedOrder2=matchedOrder[1];
                if(CheckOrder("Suite3 Case2 Check3",matchedOrder2,1,100,0,TRADER1,STOCK1,OrderBookInterface::BUY_ORDER)==false) return;

		OrderBookInterface::Order &matchedOrder3=matchedOrder[2];
                if(CheckOrder("Suite3 Case2 Check4",matchedOrder3,2,200,0,TRADER2,STOCK1,OrderBookInterface::BUY_ORDER)==false) return;

		OrderBookInterface::Order &matchedOrder4=matchedOrder[3];
                if(CheckOrder("Suite3 Case2 Check5",matchedOrder4,3,300,0,TRADER3,STOCK1,OrderBookInterface::BUY_ORDER)==false) return;

                std::queue<OrderBookInterface::Order> queue;
                if(CheckQueue("Suite3 Case2 Check6",*orderbook,STOCK1,true,0,OrderBookInterface::BUY_ORDER,queue)==false) return;

                if(!CheckSize<std::queue<OrderBookInterface::Order> >("Suite3 Case1 Check7",queue,0)) return;

        }

}

int main(int argc, char *argv[])
{	
	UnitTest::ReportInfo("Start Test");
	
	UnitTest::ReportInfo("Start Test Suite 1");
	TestSuite1();
	UnitTest::ReportInfo("End Test Suite 1");

	UnitTest::ReportInfo("Start Test Suite 2");
        TestSuite2();
        UnitTest::ReportInfo("End Test Suite 2");

        UnitTest::ReportInfo("Start Test Suite 3");
        TestSuite3();
        UnitTest::ReportInfo("End Test Suite 3");
	
	
	UnitTest::ReportInfo("End Test");
	return 0;
}

