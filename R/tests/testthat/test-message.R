context('message')

# Test the message convenience function
block <- so_SOBlock$new()

msg_content <- "What has happened?"
severity <- 5
tool_name <- "myTool"
error_name <- "TheError"

block$add_message("ERROR", tool_name, error_name, msg_content, severity)
msg <- block$TaskInformation$Message[[1]]

expect_identical(msg$type, "ERROR")
expect_identical(msg$Content, msg_content)
expect_identical(msg$Name, error_name)
expect_equal(msg$Severity, severity)
expect_identical(msg$Toolname, tool_name)