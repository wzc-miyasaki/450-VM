# EE450 :  Network TCP Project
**Student Name**:  Zecheng Wang

**ID**:  2561150344





### Code files Overview

| File Name       | What are they                                                |
| --------------- | ------------------------------------------------------------ |
| Makefile        | a make file                                                  |
| README.md       | project details                                              |
| serverM.cpp     | The implementation of ServerM                                |
| serverC.cpp     | The implementation of Credential Server                      |
| serverCS.cpp    | The implementation of CS backend server                      |
| serverEE.cpp    | The implementation of EE backend server                      |
| client.cpp      | The implementation of client server                          |
| global.cpp      | The implementation of common functions that tends to be called by all 5 servers |
| backend.cpp     | Observer Design Pattern                                      |
| courseinfo.cpp  | The class **CourseInfo** stores all the information related to a specific course code. |
| coursequery.cpp | The implementation of performing the actual Text file searching |
|                 |                                                              |
| serverM.hpp     | It defines the class **MainSev**, the main server controller |
| global.hpp      | It defines **authentication status** enumerator              |
| backend.hpp     | It defines the enumerator **Message Type** ; the class **Message** , **BackendSev**, **SubBackend** |
| courseinfo.hpp  | It defines the enumerator **CourseField** ; It defines the class **CourseInfo**; |
| coursequery.hpp | It defines the class **CourseQuery**                         |





### Completed Phases

I have implemented the following phase requirements

-   Phase 1
-   Phase 2 
-   Phase 3
-   Phase 4 
-   Extra Credits:  <u>Multiple course queries</u>





### Message exchange Format

###### Authentication Phase

-   The username and password will be saved to a buffer in a form like this: `username,password` (separated by a comma) 
-   The enumerator "**AuthFlag**" defined in `global.hpp` represents the authentication status. There are 3 **integer flags** 
    -   `0` :  the username does not exist
    -   `1` :  the username has been found, but the password is wrong
    -   `2` :  the client pass the credential authorization. 
-   The client will receive the reuslt in terms of a **null-terminated char array** (`char *` ) , and the it applies the C Standard Library function `strcmp` to parse the result. 



###### **Single Course Query**

-   The single course query will have the following format:  `course_id,course_field`. Similarly, it's separated by a comma. 
-   On the Main Server:  it first checks the first 2 character of `Course ID`
    -   If it's EE, then it forwrads it to the EE department server
    -   If it's CS, then it forwrads it to the CS department server
    -   If it's undefined department, then the main server will immediately return an empty result back to the client. 
-   On the backend server side, the department will use `Course ID` and `Course Field` as two ***keys*** to find matched course information by looking through the text files. 



###### Multiple Course Query

-   The multiple query will have the following format:  `code_1 ; code_2 ; ...`  .  Each course code is separated by the symbol `;` .  

-   On the Main Server:  it extracts all the codes from the buffer and saved to a C++ data structure **vector**.  For each course code, the main server recreate the query in following format:  `course_code,All` 

    >   `All` is one of the course field type defined in `courseinfo.hpp`.  It means the server should return the full course information of the course that matches with the `course_code`.

-    When all the query result is received, it transmits them back to the client all at once.  The query results has following format:  `Result_1#Result_2#...#`  .  Each result is separated by the symbol `#`. 





### Code References in my project

In my project, I follow the Beej's Tutorial to build up both **TCP and UDP servers and clients.** Most of the references are from Beej's two examples: ***Stream Socket and Datagram Socket*** in chapter 6.  I heavily relies on the function `getaddrinfo`  to create the socket, which is an elegant approach introduced in in *Beej's Book* to create socket addresses. 





### Additional Code Files Details

**Overview:** 

-   `serverM.hpp`

    >   The hearder file that defines the main server controler class **MainSev** to control the behavior of the main server. 



-   `global.cpp`  ;  `global.hpp`

    -   the `global` headerfile includes some common C library Headerfiles that required for *Socket Programming*. 

    -   It has some common functions that tends to be called by all 5 servers. For convenience, I collect those functions inside `global` file. 

    -   The **Authentication status** enumerator is defined bere. 

    

-   `courseinfo.cpp`  ;  ``courseinfo.hpp``

    >   The implementation of the class **CourseInfo**, which maintains the complete course informations for each course code. The purpose is to help in the **query searching** process in backend servers (**EE & CS**).  

    

-   `backend.cpp`  ;  `backend.hpp` 

    -   It defines a class named **Message** 

    -   It defines an enumerator **Msg_type** to define different *Message type* 

    -   It implements **Observer** design pattern 
        -   A subject class **BackendSv**

        -   3 Observer classes **SubBackend** for each backend server (Credential, EE and CS). 


    

-   `courseQuery.cpp`  ;  `courseQuery.hpp`

    >   The implementation of the class **CourseQuery**,  The CourseQuery instance is initialised with a char array containing **the target file path **. The purpose is to help in the **text file searching** in backend servers (**EE & CS**). It performs the searching with the assistance of the **CourseInfo** class. 





### Linux Command

```bash
sudo netstat -tunlp
```
- list all TCP or UDP ports that are being listened on, including the services using the ports and the socket status

- `-t` - Show TCP ports.
- `-u` - Show UDP ports.
- `-n` - Show numerical addresses instead of resolving hosts.
- `-l` - Show only listening ports.
- `-p` - Show the PID and name of the listener’s process. This information is shown only if you run the command as root or sudo user.

[reference](https://linuxize.com/post/check-listening-ports-linux/) 
