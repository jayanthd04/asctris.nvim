-- local exec_path = "C:\\Users\\jayan\\Documents\\CPP\\tetris\\tetrisMult.exe"
-- local res = os.execute('"' .. exec_path .. '"')
local M = {}

--[=====[
local function get_plugin_dir()
    local plugin_name = 'asctrisPlugin'
    local rt_files = vim.api.nvim_get_runtime_file("lua/" .. plugin_name .. "/init.lua", false)

    if #rt_files <1 then 
        error("Can't find Asctris Plugin")
    end 

    local plugin_path = rt_files[1]
    -- local plugin_dir = vim.fn.fnamemodify(plugin_path, ":h:h:h")
    -- plugin dir for now without cleaned up directory struct for neovim plugin 
    local plugin_dir = vim.fn.fnamemodify(plugin_path, ":h:h:h:h")
    return plugin_dir
end
print(get_plugin_dir())
--]=====]
-- local plugin_dir = require("asctrisPlugin").plugin_dir()
-- print(plugin_dir)
local plugin_path = require("asctris.path").plugin_dir()
-- print(plugin_path)
local exec_path = plugin_path .. "/source/asctrisExec"
M.run_game = function ()
    -- vim.cmd('term ' .. exec_path)
    -- vim.cmd('startinsert')
    -- local width = vim.o.columns -10
    -- local plugin = require("asctrisPlugin.path")
    -- local plugin_path = require("asctrisPlugin.path").plugin_dir()
    -- print(plugin_path)
    -- exec_path = plugin_path .. "/tetrisMult.exe" 
    local width = 12
    -- local height = vim.o.lines -10
    local height = 17
    -- local row = 5
    local row = (vim.o.lines/2) - 6
    -- local col = 5
    local col = (vim.o.columns/2) -8

    local buf = vim.api.nvim_create_buf(false,true)
    vim.api.nvim_buf_set_option(buf,'scrollback',10000) 
    vim.opt_local.updatetime = 16
    --vim.api.nvim_buf_set_option(buf,'updatetime',16)
    local opts = {
        style = 'minimal',
        relative = 'editor',
        width = width,
        height = height,
        row = row, 
        col = col,
        -- updatetime=16,
        -- scrollback = 1000,
    }
    local win = vim.api.nvim_open_win(buf, true, opts)
    -- vim.api.nvim_win_set_option(win, 'updatetime', 16)
    -- vim.api.nvim_win_set_option(win, 'scrollback', 10000)
    local function on_exit(job_id,exit_code,event)
        vim.schedule(function()
            -- vim.cmd('stopinsert')
            vim.api.nvim_win_close(win,true)
        end)
    end 
    vim.fn.termopen(exec_path,{
        on_exit = on_exit 
    })

    vim.cmd('startinsert')

    -- print(plugin_path)
end 
M.setup = function ()
    local function asctris_dispatcher(args)
        local func_name = args.fargs[1]
        if func_name and asctris[func_name] then 
            asctris[func_name]()
        else 
            print("Function ".. func_name .. " is invalid for Asctris")
        end 
    end 
    vim.api.nvim.create_user_command('Asctris',asctris_dispatcher,{nargs = 1})
end
--[[return {
    run_game = run_game 
}]]--
return M
