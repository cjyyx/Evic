# 库Evic
类似TinyExpr

# 函数
见Evic.h

# 求值过程
- EVIC_cut:将中缀表达式切割成一个个字符或数字对象
- EVIC_valueLink:将表达式的字符与变量联系
- EVIC_funLink:将表达式的字符与函数联系
- EVIC_compile:完成字符的联系，转换为后缀表达式，返回表达式对象
- EVIC_calculate:对表达式计算