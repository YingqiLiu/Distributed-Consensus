Yingqi Liu     5200537875

My codes are Proposer.cpp, Acceptor.cpp, and Learner.cpp.
Proposer randomly create proposal and sends it to acceptors, and depending on the responces from acceptors, proposer send accept request to acceptors.
Acceptor receives proposals from proposers and decides which one it will accept and send the responces back to proposer, when it receive accept request from proposers, it sends the result to learner.
Leaner receives result from acceptors, when majority of acceptors send the same result, learner accept it as the final result.

1. Compile the codes. Use command "g++ -o learner Learner.cpp -lsocket -lnsl" to compile Learner.cpp, use command "g++ -o acceptor Acceptor.cpp -lsocket -lnsl" to compile Acceptor.cpp, use command "g++ -o proposer Proposer.cpp -lsocket -lnsl" to compile Proposer.cpp.
2. Open three windows, enter "./learner" in the first window to run learner, it will show "Enter number of acceptors: " on screen and then enter the number of acceptors.
3. Enter "./proposer" in the second window to run proposer, it will show "Enter number of acceptors: " and then enter the number of acceptors, then it will show "Enter number of proposers: ", enter the number of proposers on.
4. Enter "./acceptor" in the third window to run acceptor, it will show "Enter number of acceptors: " and then enter the number of acceptors, then it will show "Enter number of proposers: ", enter the number of proposers on.
5. The result will show on the screens.

In my codes proposers don't start to send proposals at the same time so the first proposal gotten by acceptors is always sent by the first proposer, I don't know how to solve this problem.
