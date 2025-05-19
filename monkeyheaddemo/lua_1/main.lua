function love.load()
    math.randomseed(os.time())
    love.window.setMode(1024, 768, {resizable = true, vsync = true, minwidth = 800, minheight = 600})

    love.graphics.setBackgroundColor(0.1, 0.1, 0.1)
    local numPolygons = 5000
    local screenWidth, screenHeight = love.graphics.getDimensions()
    ellipseWidth, ellipseHeight = 800, 650

    local polygons = {}

    for i = 1, numPolygons do
        local numSides = math.random(3, 5) -- Random number of sides between 3 and 5
        local angle = math.random() * 2 * math.pi
        local x = screenWidth / 2 + (ellipseWidth / 2) * math.cos(angle)
        local y = screenHeight / 2 + (ellipseHeight / 2) * math.sin(angle)
        local offsetRand = math.random(1,13) 
        local offsetX,offsetY = x - screenWidth/2, y - screenHeight/2
        offsetX,offsetY = normalizeVector(offsetX,offsetY)
        local offset = 0
        if offsetRand < 2.0 then offset = 20 end
        if offsetRand > 2.0 and offsetRand < 4.5 then offset = 16 end
        if offsetRand > 4.5 and offsetRand < 7.0 then offset = 11 end
        if offsetRand > 7   then offset = 7  end
        if offsetRand > 9   then offset = 4  end
        x = x + offset*offsetX
        y = y + offset*offsetY
        local polyAngle = math.random() * 2 * math.pi
        local vertices = {}
        for j = 1, numSides do
            local vertexAngle = (j - 1) * (2 * math.pi / numSides) + polyAngle
            table.insert(vertices, math.cos(vertexAngle) * 8)
            table.insert(vertices, math.sin(vertexAngle) * 8)
        end
        table.insert(polygons, {x = x, y = y, vertices = vertices, angle = polyAngle})
    end

    love.graphics.setLineWidth(1)
    -- Store the polygons in a global variable
    _polygons = polygons
end

function normalizeVector(x, y)
  local magnitude = math.sqrt(x * x + y * y)
  if magnitude == 0 then
    return 0, 0  -- or handle this case differently, e.g. return an error
  end
  return x / magnitude, y / magnitude
end

function dist_inner_ring(x, y)
    local a = ellipseWidth / 2
    local b = ellipseHeight / 2
    local ellipseDist = math.sqrt((x^2 / a^2) + (y^2 / b^2))
    return math.abs(ellipseDist - 1) * 100 -- Scale the distance for coloring
end

function love.draw()
    local screenWidth, screenHeight = love.graphics.getDimensions()
    for _, poly in ipairs(_polygons) do
        love.graphics.push()
        local dist_cent = dist_inner_ring(poly.x - screenWidth / 2, poly.y - screenHeight / 2)
        local brightness = math.max(0, 1 - dist_cent / 20) -- Adjust brightness based on distance
        local bright8 = brightness^8
        love.graphics.setColor(bright8, bright8, bright8/1.8) -- Set color to white based on distance
        love.graphics.translate(poly.x, poly.y)
        love.graphics.rotate(poly.angle)
        love.graphics.polygon("fill", poly.vertices)
        love.graphics.pop()
    end
end

function love.update(dt)
    -- Update logic here
end

function love.keypressed(key)
    if key == "space" then
        -- Play the sound when the space key is pressed
        -- sound:play()
    end
end
