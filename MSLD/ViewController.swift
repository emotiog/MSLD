//
//  ViewController.swift
//  MSLD
//
//  Created by junsang on 2021/01/09.
//

import UIKit
import AVFoundation

class ViewController: UIViewController, AVCaptureVideoDataOutputSampleBufferDelegate {
    private var captureSession: AVCaptureSession = AVCaptureSession()
    private let videoDataOutput = AVCaptureVideoDataOutput()
    
    override func viewDidLoad() {
        super.viewDidLoad()
        // Do any additional setup after loading the view.
        self.addCameraInput()
        self.getFrames()
        self.captureSession.startRunning()
    }

    override func didReceiveMemoryWarning() {
        super.didReceiveMemoryWarning()
    }
    
    private func addCameraInput() {
        guard let captureDevice = AVCaptureDevice.DiscoverySession(deviceTypes: [.builtInWideAngleCamera, .builtInDualCamera, .builtInTrueDepthCamera], mediaType: .video, position: .back).devices.first else {
            fatalError("No back camera device found, please make sure to run MSLD in an iOS device and not a simulator")
        }
        let cameraInput = try! AVCaptureDeviceInput(device:captureDevice)
        self.captureSession.addInput(cameraInput)
    }
    
    func captureOutput(_ output: AVCaptureOutput, didDrop sampleBuffer: CMSampleBuffer, from connection: AVCaptureConnection) {
        print("did receive frame")
    }
    
    private func getFrames() {
        // Any? https://zeddios.tistory.com/213
        // Any는 함수 타입을 포함하여 모든 타입의 인스턴스를 나타낼 수 있습니다.
        // AnyObject 모든 class 타입의 인스턴스를 나타낼 수 있습니다.
        
        // as: type casting https://gaki2745.github.io/swift/2019/10/02/Swift-Basic-04/
        // 타입 checking : is
        videoDataOutput.videoSettings = [(kCVPixelBufferPixelFormatTypeKey as NSString) : NSNumber(value: kCVPixelFormatType_32BGRA)] as [String : Any]
        
        videoDataOutput.alwaysDiscardsLateVideoFrames = true
        videoDataOutput.setSampleBufferDelegate(self, queue: DispatchQueue(label:"camera.frame.processing.queue"))
        self.captureSession.addOutput(videoDataOutput)
        
        // if 문에서의 , https://dev200ok.blogspot.com/2020/09/swift-if.html
        // if let result = result && result { print("true") } <- error
        // if let 으로 옵셔널을 제거한 뒤 사용 가능 -> comma(,)를 이용해 코드가능
        // let result: Bool? = false
        // if let result = result, result { print("true") } else { print("false") }
        // if let result = result && result { : 하나의 expression
        // if let result = result, result { // expression, expression 으로 이루어진 condition-list
        guard let connection = self.videoDataOutput.connection(with: AVMediaType.video), connection.isVideoOrientationSupported else { return }
        connection.videoOrientation = .portrait
    }
}
