SET obj = CreateObject("CSExeCOMServer.CSSimpleObject")

MsgBox "A CSExeCOMServer.CSSimpleObject object is created"

' ִ��HelloWorld����������һ���ַ���
MsgBox "The HelloWorld method returns " & obj.HelloWorld

' ����FloatProperty����
MsgBox "Set the FloatProperty property to 1.2"
obj.FloatProperty = 1.2

'��ȡFloatProperty����
MsgBox "The FloatProperty property returns " & obj.FloatProperty

SET obj = Nothing