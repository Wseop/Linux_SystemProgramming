# Linux_SystemProgramming
시스템 프로그래밍 실습 과제
## 1. Execlp 활용
### 1.1 구현내용
git repository에 있는 파일들을 검색하여 vim으로 실행시켜준다.
### 1.2 실행방법
1. git repository에서 검색하고자 하는 keyword와 함께 실행
```
../out/exec_vim keyword
```
2. 파일 목록이 뿌려지면 숫자로 파일을 선택하여 실행
3. 숫자 대신 키워드 입력 시 결과내에서 재검색
## 2. FD_SET과 select()를 활용한 채팅 프로그램
### 2.1 구현내용
다수의 클라이언트 간 채팅이 가능한 프로그램

서버는 클라이언트의 메세지를 받아서 다른 모든 클라이언트에게 broadcasting 해준다.
### 2.2 실행방법
1. server : port number를 인자로 넘겨주면서 실행
```
./server 8001
```
2. client : port number와 ip주소를 인자로 넘겨주면서 실행
```
./client 8001 127.0.0.1
```
