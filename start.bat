@echo off
if not exist ".\node_modules" (
    npm i
    npm start
) else (
    npm start
)