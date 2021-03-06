:
eval 'exec perl -S $0 ${1+"$@"}'
    if $running_under_some_shell;
##
##  petidomo-approve -- Petidomo approval tool
##  Copyright (c) 2000 Ralf S. Engelschall <rse@engelschall.com>
##

#   locate the file with the approval passwords
my $config = "$ENV{HOME}/.petidomo";

#   locate the Sendmail program
my $sendmail = "sendmail";
foreach my $dir (split(/:/, "$ENV{PATH}:/bin:/sbin:/usr/bin:/usr/sbin:/lib:/usr/lib")) {
    if (-x "$dir/sendmail") {
        $sendmail = "$dir/sendmail";
        last;
    }
}

#   suck in the whole mail
my $mail = '';
$mail .= $_ while (<STDIN>);

#   dispatch over the two mail types which need approval...
if ($mail =~ m|^Subject:\s+Petidomo:\s+APPROVE\s+(\S+):|mi) {
    #   approve a request
    my $list = $1;
    my $replyto = 'petidomo';
    $replyto = $1 if ($mail =~ m|^Reply-to:\s+(\S+)|mi);
    my $password = 'petidomo';
    open(FP, "<$config");
    while (<FP>) {
        next if (m|^\s*#.*| or m|^\s*$|);
        $password = $1 if (m|^\s*(?:$list)\s+(\S+)|);
    }
    close(FP);
    $mail =~ s|^.+?\n(password\s+<AdminPassword>.+?)\n\n.*|$1|s;
    $mail =~ s|<AdminPassword>|$password|s;
    open(SM, "|$sendmail $replyto") || die "cannot spawn $sendmail";
    print SM "To: $replyto\n\n";
    print SM $mail;
    close(SM);
    print STDERR "petidomo-approve: sent approved mail back to $replyto\n";
    sleep(1);
    exit(0);
}
elsif ($mail =~ m|^Subject:\s+Petidomo:\s+BOUNCE|mi) {
    #   approve a posting
    my $list = 'petidomo';
    $list = $1 if ($mail =~ m|^Subject:\s+Petidomo:\s+BOUNCE\s+(\S+):|m);
    my $replyto = $list;
    my $password = 'petidomo';
    open(FP, "<$config");
    while (<FP>) {
        next if (m|^\s*#.*| or m|^\s*$|);
        $password = $1 if (m|^\s*(?:$list)\s+(\S+)|);
    }
    close(FP);
    $mail =~ s|^.+?\n\n||s;
    $mail =~ s|^.+?\n>?From .+?\n||s;
    $mail =~ s|Delivered-To:\s*$list\s*\n||s;
    $mail = "Approved: $password\n" . $mail; # best
    #$mail =~ s|^(.+?\n\n)(.*)$|$1Approved $password\n$2|s; # not good
    open(SM, "|$sendmail $replyto") || die "cannot spawn $sendmail";
    print SM $mail;
    close(SM);
    print STDERR "petidomo-approve: sent approved mail back to $replyto\n";
    sleep(1);
    exit(0);
}
else {
    print STDERR "petidomo-approve: unrecognized mail type\n";
    sleep(1);
    exit(1);
}
