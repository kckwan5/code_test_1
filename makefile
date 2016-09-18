default: order_book_test

clean:
	rm order_book_test
order_book_test: order_book.cpp order_book_test.cpp
	
	g++ -std=c++0x -o order_book_test order_book_test.cpp order_book.cpp


