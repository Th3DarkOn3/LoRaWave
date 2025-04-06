import { ComponentFixture, TestBed, waitForAsync } from '@angular/core/testing';

import { ModifyDeviceFormComponent } from './modify-device-form.component';

describe('ModifyDeviceFormComponent', () => {
  let component: ModifyDeviceFormComponent;
  let fixture: ComponentFixture<ModifyDeviceFormComponent>;

  beforeEach(waitForAsync(() => {
    TestBed.configureTestingModule({
      imports: [ModifyDeviceFormComponent],
    }).compileComponents();

    fixture = TestBed.createComponent(ModifyDeviceFormComponent);
    component = fixture.componentInstance;
    fixture.detectChanges();
  }));

  it('should create', () => {
    expect(component).toBeTruthy();
  });
});
