-- quest_1001.lua — квест «Первые шаги»
-- Квест: убить 5 слизней (entry 50001)

quest.on("accept", function(player)
    player:send_message("Квест принят: убейте 5 слизней.")
end)

quest.objective_kill("creature", 50001, 5)

quest.on("complete", function(player)
    player:add_experience(100)
    player:add_item(10851, 1) -- зелье
    player:send_message("Квест выполнен!")
end)
