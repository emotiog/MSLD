//
//  ResultViewContoller.swift
//  MSLD
//
//  Created by USER on 2021/05/15.
//

import Foundation

class ResultViewController : ViewController {
    
    var originImage : UIImage!
    var resultImage : UIImage!
    var imageView: UIImageView!
    
    let pngFileManager: PNGFileManager = PNGFileManager()
    
    override func viewDidLoad() {
        super.viewDidLoad()
        self.imageView = buildImageView()
        self.view.addSubview(self.imageView)
        var position = CGPoint(x: self.view.frame.width * 0.8971, y: 10)
        self.view.addSubview(buildSaveButton(position: position))
        position.x += 10
        self.view.addSubview(buildBackButton(position: position))
        
        self.imageView.image = originImage
    }
    
    //MARK: private function
    private func buildImageView() -> UIImageView {
        let view: UIImageView = UIImageView(frame: CGRect(x: 0, y: 0, width: 480, height: 640))
        view.layer.position = CGPoint(x: self.view.frame.width * 0.5, y: self.view.frame.height * 0.5)
        return view
    }
    
    private func buildBackButton(position: CGPoint) -> UIButton {
        let button = UIButton(frame: CGRect(x: 0, y: 0, width: 80, height: 40), primaryAction: UIAction(title: "back", handler: { _ in self.dismiss(animated: true, completion: nil)}))

        button.backgroundColor = UIColor.gray
        button.layer.masksToBounds = true
        button.layer.cornerRadius = 20.0
        button.layer.position = position
        return button
    }
    
    private func buildSaveButton(position: CGPoint) -> UIButton {
        
        let action = UIAction(title: "Save", handler: { _ in
            let uuid = UUID().uuidString
            
            self.pngFileManager.save(image: self.originImage, forKey: uuid)
            OpenCVWrapper.appendDesc(self.originImage, savePath: uuid)
            
            self.messageBox(title: "Success", message: "Save new image successfully", messageBoxStyle: .alert, alertActionStyle: .cancel) {
                self.dismiss(animated: true, completion: nil)
            }
        })
        
        let button = UIButton(frame: CGRect(x: 0, y: 0, width: 80, height: 40), primaryAction: action)
        
        button.backgroundColor = UIColor.red
        button.layer.masksToBounds = true
        button.layer.cornerRadius = 20.0
        button.layer.position = position
        return button
    }
    
    // @escaping 클로저는 함수의 인자로 전달되었을 때, 함수의 실행이 종료된 후 실행되는 클로저 -> 함수 밖(escaping)에서 실행되는 클로저
    // 반대 -> Non-Escaping 클로저
    // https://jusung.github.io/Escaping-Closure/
    private func messageBox(title: String, message: String, messageBoxStyle: UIAlertController.Style, alertActionStyle: UIAlertAction.Style, completionHandler: @escaping () -> Void) {
        
        
        let okAction = UIAlertAction(title: "Ok", style: alertActionStyle, handler: { _ in completionHandler() // 이건 okay 누른 이후에 비동기 식으로 작동
        })
        let alert = UIAlertController(title: title, message: message, preferredStyle: messageBoxStyle)
        
        alert.addAction(okAction)
        
        present(alert, animated: true, completion: nil)
    }
}
