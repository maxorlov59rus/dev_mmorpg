lines = open('C:/Users/Orel/AppData/Local/Cline/dev_mmorpg/dm-server/src/game/Network/Handlers/PacketHandlers.cpp', 'r', encoding='utf-8').readlines()
idx = None
for i in range(270, len(lines)):
    if lines[i].strip() == '}':
        idx = i
        break
if idx is not None:
    lines = lines[:idx+1] + ['\n', '} // namespace net\n', '} // namespace dm\n']
    open('C:/Users/Orel/AppData/Local/Cline/dev_mmorpg/dm-server/src/game/Network/Handlers/PacketHandlers.cpp', 'w', encoding='utf-8').writelines(lines)
    print('Done, truncated to', len(lines), 'lines')
else:
    print('Pattern not found')
