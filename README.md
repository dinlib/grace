
# Czin

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
- [ ] stmtIf          

#### While statement
- [ ] stmtWhile       

#### For statement
- [ ] stmtFor         

#### Loop interrupt statement
- [ ] stmtStop        

#### Loop skip statement
- [ ] stmtSkip        

#### Return statement
- [ ] stmtReturn     

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
- [ ] Mult, Div, Mod  
- [ ] Add, Sub        
- [ ] MT, MEQT        
- [ ] LT, LEQT        
- [ ] EQ, DIFF        
- [ ] AND              
- [ ] OR              
- [ ] Ternary OP      
