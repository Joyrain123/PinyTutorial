# Bsp
using CRTP to Implement a singleton bsp class  
## Advantages
1. Thread safety  
2. High performance, no virtual function consumption  
3. Decoupling design  

## Bsp Can
请使用以下函数注册fdcan接收新消息的回调函数  
```
Can::instance().registerCallback(&hfdcan1, 0x201, canCallback);
```

## Bsp Uart
请使用以下函数注册uart回调函数  
```
Uart::instance().registerCallback(huart, &Rc::callBackFromISR);
```
