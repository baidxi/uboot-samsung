# SPDX-License-Identifier: GPL-2.0+
#
# Copyright 2021 Google, Inc
#
# SPDX-License-Identifier:	GPL-2.0+
#
# Awk script to parse a text file containing an environment and convert it
# to a C string which can be compiled into U-Boot.

# The resulting output is:
#
#   #define CONFIG_EXTRA_ENV_TEXT "<environment here>"
#
# If the input is empty, this script outputs a comment instead.

BEGIN {
	# env holds the env variable we are currently processing
	env = "";
	ORS = ""
}

# Skip empty lines, as these are generated by the clang preprocessor
NF {
	do_output = 0

	# Quote quotes
	gsub("\"", "\\\"")

	# Avoid using the non-POSIX third parameter to match(), by splitting
	# the work into several steps.
	has_var = match($0, "^([^ \t=][^ =]*)=(.*)$")

	# Is this the start of a new environment variable?
	if (has_var) {
		if (length(env) != 0) {
			# Record the value of the variable now completed
			vars[var] = env
			do_output = 1
		}

		# Collect the variable name. The value follows the '='
		match($0, "^([^ \t=][^ =]*)=")
		var = substr($0, 1, RLENGTH - 1)
		env = substr($0, RLENGTH + 1)

		# Deal with += which concatenates the new string to the existing
		# variable. Again we are careful to use POSIX match()
		if (length(env) != 0 && match(var, "^(.*)[+]$")) {
			plusname = substr(var, RSTART, RLENGTH - 1)
			# Allow var\+=val to indicate that the variable name is
			# var+ and this is not actually a concatenation
			if (substr(plusname, length(plusname)) == "\\") {
				# Drop the backslash
				sub(/\\[+]$/, "+", var)
			} else {
				var = plusname
				env = vars[var] env
			}
		}
	} else {
		# Change newline to space
		gsub(/^[ \t]+/, "")

		# Don't keep leading spaces generated by the previous blank line
		if (length(env) == 0) {
			env = $0
		} else {
			env = env " " $0
		}
	}
}

END {
	# Record the value of the variable now completed. If the variable is
	# empty it is not set.
	if (length(env) != 0) {
		vars[var] = env
		do_output = 1
	}

	if (do_output) {
		printf("%s", "#define CONFIG_EXTRA_ENV_TEXT \"")

		# Print out all the variables
		for (var in vars) {
			env = vars[var]
			print var "=" vars[var] "\\0"
		}
		print "\"\n"
	}
}
