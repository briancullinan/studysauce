//
//  SignUp2ViewController.m
//  Study Sauce Official
//
//  Created by Harry Alberti on 2/22/14.
//  Copyright (c) 2014 Harry Alberti. All rights reserved.
//

#import "SignUp2ViewController.h"
#import <QuartzCore/QuartzCore.h>

@interface SignUp2ViewController ()

@property (weak, nonatomic) IBOutlet UITextField *firstName;
@property (weak, nonatomic) IBOutlet UITextField *PasswordField;
@property (weak, nonatomic) IBOutlet UITextField *lastName;
@property (weak, nonatomic) IBOutlet UITextField *emailField;
@property (weak, nonatomic) IBOutlet UIWebView *text;

@end

@implementation SignUp2ViewController

- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil
{
    self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
    if (self) {
        // Custom initialization
    }
    return self;
}

- (void)viewDidLoad
{
    [_firstName becomeFirstResponder];
    [_firstName setDelegate:(id)self];
    [_lastName setDelegate:(id)self];
    [_emailField setDelegate:(id)self];
    [_PasswordField setDelegate:(id)self];
    [_text setDelegate:(id)self];
    _firstName.layer.borderColor=[[UIColor colorWithRed:0.0/255.0
                                                  green:153.0/255.0
                                                   blue:190.0/255.0
                                                  alpha:1.0] CGColor];
    _firstName.layer.borderWidth= 1.5f;
    _lastName.layer.borderColor=[[UIColor colorWithRed:0.0/255.0
                                                  green:153.0/255.0
                                                   blue:190.0/255.0
                                                  alpha:1.0] CGColor];
    _lastName.layer.borderWidth= 1.5f;
    _emailField.layer.borderColor=[[UIColor colorWithRed:0.0/255.0
                                                  green:153.0/255.0
                                                   blue:190.0/255.0
                                                  alpha:1.0] CGColor];
    _emailField.layer.borderWidth= 1.5f;
    _PasswordField.layer.borderColor=[[UIColor colorWithRed:0.0/255.0
                                                  green:153.0/255.0
                                                   blue:190.0/255.0
                                                  alpha:1.0] CGColor];
    _PasswordField.layer.borderWidth= 1.5f;
    
    NSString *urlAddress = [[NSBundle mainBundle] pathForResource:@"test" ofType:@"rtf"];
    NSURL *url = [NSURL fileURLWithPath:urlAddress];
    NSURLRequest *requestObj = [NSURLRequest requestWithURL:url];
    [_text loadRequest:requestObj];
    _text.scrollView.scrollEnabled = NO;
    _text.scrollView.bounces = NO;
    _text.multipleTouchEnabled = NO;
    
    [self.view addSubview:_firstName];
    [super viewDidLoad];
    
	// Do any additional setup after loading the view.
}

-(BOOL)textFieldShouldReturn:(UITextField*)textField;
{
    _firstName.tag = 0;
    _lastName.tag = 1;
    _emailField.tag = 2;
    _PasswordField.tag = 3;
    NSInteger nextTag = textField.tag + 1;
    // Try to find next responder
    UIResponder* nextResponder = [textField.superview viewWithTag:nextTag];
    if (nextResponder) {
        // Found next responder, so set it.
        [nextResponder becomeFirstResponder];
    } else {
        // Not found, so remove keyboard.
        [textField resignFirstResponder];
    }
    return NO; // We do not want UITextField to insert line-breaks.
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}
@end
