Search help:
        USAGE: search [OPTION]... TERM FILE

        -h, --help              Show this help dialog

        -i, --ignore-case       Search is not case sensitive

        -I, --isolate           Only return a word where it is
                                an exact match, ie, not part of
                                a compound word or hyphenated
                                word.

        -l, --lines             Display line numbers in results
                                and the position of the start of
                                the word

        -r, --range             Display results only from a
                                given range of lines in the file.
                                Used in the format of num-num.
                                EG: -r 50-75

        -R, --remove-dupes      Only shows the a line once
                                regardless of how many times the
                                search term appears in the line.
                                Not yet implemented.

        -s, --save              Save results to a file

        Search is able to be used with other programms.
        Try piping the results of search into
        grep to narrow down search results

        EG: search Port /etc/ssh/sshd_config | grep 22
