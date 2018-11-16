# Grace Lang

## Installing Bison
Install Bison 3.1 from this [link](http://ftp.gnu.org/gnu/bison/bison-3.1.tar.xz).

Use the following commands to install:
```bash
tar -xvf bison-3.1.tar.xz
cd bison-3.1/
./configure && make && make install
```

## Installing Flex 
Install Flex using `sudo apt-get install flex` (We are using Flex v.2.6.0).

## Running the project
Clone the repository at `git clone https://github.com/grsouza/czin.git`.
```bash
cd czin
make
./czin name-of-file.cz
```
## Tasks
### Program
- [X] program        

### Variables
- [X] decVar          
- [X] listSpecVar     
- [X] specVar         
- [ ] UsingVariable   

#### Procedure
- [X] decProc         

#### Function
- [X] decFunc         

#### Parameters
- [ ] ParamsList      
- [ ] SpecParams      
- [ ] param           

### Comands
- [ ] stmt            

#### Assignment
- [ ] stmtAtrib       
- [ ] atrib           

#### If statement
- [X] stmtIf          

#### While statement
- [X] stmtWhile       

#### For statement
- [X] stmtFor         

#### Loop interrupt statement
- [X] stmtStop        

#### Loop skip statement
- [X] stmtSkip        

#### Return statement
- [X] stmtReturn     

#### Procedure Call Statement
- [ ] stmtCallProc    

#### Read Statement
- [ ] stmtRead        

#### Write Statement
- [ ] stmtWrite       

##### Statement Block
- [ ] block           

### Expression
- [ ] Negative
- [ ] NOT        
- [X] Mult, Div, Mod  
- [X] Add, Sub        
- [X] GT, GEQT        
- [X] LT, LEQT        
- [X] EQ, DIFF        
- [X] AND              
- [X] OR              
- [ ] Ternary OP      
