from lib2to3.pgen2.token import tok_name
from pywechat.WeChatMsg.WeChatTextRecv import WeChatTextRecv
from pywechat.WeChatMsg.WeChatFifo import WeChatFifo
from pywechat.RobotApp.Weather import Weather
import time

msg = WeChatTextRecv()
fifo = WeChatFifo()

print("\nStart Wechat Robot\n")

my_chatroom_name = 's0duku'
# attention ! The wxid must start with wxid_, this is your true wxid。
my_wxid = 'filehelper'

chatroom_payload = '@{}\u2005'.format(my_chatroom_name)


def robot_loop():
    while True:
        id,wxid,text = msg.recvText()
        if not id:
            continue
        _,nickname,_ = fifo.userInfo(wxid)
        if text.startswith(chatroom_payload) or id == my_wxid:
            if text.startswith(chatroom_payload):
                text = text[len(chatroom_payload):]
            print("[REPLY TO] {}: {}".format(nickname,text))
            fifo.sendText(id,robot_exec(text))
        else:
            print("[NORMAL MSG] {}: {}".format(nickname,text))


def robot_exec(cmd):
    tokens = [x for x in cmd.split(' ') if x != '']
    if not tokens:
        return '111'
    if tokens[0] == '天气':
        if len(tokens) < 2:
            return '"天气 [地点]" 加上地点，我才能帮你获取天气！'
        return repr(Weather(tokens[1]))
    elif tokens[0] == '时间':
        return '{} 脱裤子放屁？你不是在用手机吗？'.format(time.strftime("%Y-%m-%d %H:%M:%S", time.localtime()))
        
    return '111'

if __name__ == '__main__':
    robot_loop()